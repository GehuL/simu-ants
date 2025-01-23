#include "Mutator.h"
#include "Genome.h"
#include"neuron_mutator.h"
#include "link_mutator.h"
#include "RNG.h"
#include <iostream>
#include <functional>
#include <algorithm>
#include <queue>    


void Mutator::mutate(Genome &genome, const NeatConfig &config, RNG &rng) {
    // Probabilité d'une mutation structurelle
    if (rng.next_double() < config.probability_structure_mutation) {
        // Liste des mutations structurelles possibles et de leurs probabilités relatives
        std::vector<std::pair<std::function<void(Genome&)>, double>> structure_mutations = {
            {mutate_add_link_fix, config.probability_add_link},
            {mutate_remove_link_fix, config.probability_remove_link},
            {mutate_add_neuron_fix, config.probability_add_neuron},
            {mutate_remove_neuron_fix, config.probability_remove_neuron}
        };

        // Normaliser les probabilités pour éviter des erreurs de cumul
        double total_probability = 0.0;
        for (const auto& mutation : structure_mutations) {
            total_probability += mutation.second;
        }

        double mutation_choice = rng.next_double() * total_probability;

        // Sélectionner et appliquer une mutation structurelle
        double cumulative_probability = 0.0;
        for (const auto& mutation : structure_mutations) {
            cumulative_probability += mutation.second;
            if (mutation_choice < cumulative_probability) {
                mutation.first(genome); // Appliquer la mutation structurelle
                break;
            }
        }
    }

    if (rng.next_double() < config.probability_weight_or_bias_mutation) {
    if (rng.next_bool()) {
        mutate_link_weight(genome, config, rng);
    } else {
        mutate_neuron_bias(genome, config, rng);
    }
}

}


void Mutator::mutate_add_link(Genome &genome) { 
    int input_id = choose_random_input_or_hidden_neuron(genome.get_neurons());  
    int output_id = choose_random_output_or_hidden_neuron(genome.get_neurons());

    if (input_id == -1 || output_id == -1) {
        return;
    }

    neat::LinkId link_id{input_id, output_id};

    auto existing_link = genome.find_link(link_id);
    if (existing_link) {
        if (!existing_link->is_enabled) {
            existing_link->is_enabled = true;
        }
        return;
    }

    if (would_create_cycle(genome.get_links(), input_id, output_id)) {
        return;
    }

    neat::LinkMutator link_mutator;
    neat::LinkGene new_link = link_mutator.new_value(input_id, output_id);
    genome.add_link(new_link);
    std::cout << "Adding link from " << input_id << " to " << output_id << " to genome " << genome.get_genome_id() << std::endl;

}

void Mutator::mutate_add_link_fix(Genome &genome) {
    int input_id = choose_random_input_or_hidden_neuron(genome.get_neurons());
    int output_id = choose_random_output_or_hidden_neuron(genome.get_neurons());

    if (input_id == -1 || output_id == -1 || input_id == output_id) {
        return; // Identifiants invalides ou identiques
    }

    neat::LinkId link_id{input_id, output_id};

    auto existing_link = genome.find_link(link_id);
    if (existing_link) {
        if (!existing_link->is_enabled) {
            existing_link->is_enabled = true; // Réactive le lien désactivé
        }
        return;
    }

    if (would_create_cycle(genome.get_links(), input_id, output_id)) {
        return; // Évite la création de cycles
    }

    neat::LinkMutator link_mutator;
    neat::LinkGene new_link = link_mutator.new_value(input_id, output_id);
    genome.add_link(new_link);

    std::cout << "Adding link from " << input_id << " to " << output_id << " to genome " << genome.get_genome_id() << std::endl;
}


void Mutator::mutate_remove_link(Genome &genome) {
    RNG rng;
    NeatConfig config;

    if (genome.get_links().empty()) {
        return;
    }

    std::unordered_set<neat::LinkId, neat::LinkIdHash> essential_links;

    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id < config.num_inputs) {
            for (const auto& link : genome.get_links()) {
                if (link.link_id.input_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id >= config.num_inputs && 
            neuron.neuron_id < config.num_inputs + config.num_outputs) {
            for (const auto& link : genome.get_links()) {
                if (link.link_id.output_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    for (const auto& link : genome.get_links()) {
        if (link.link_id.input_id >= config.num_inputs && link.link_id.output_id >= config.num_inputs) {
            essential_links.insert(link.link_id);
        }
    }

    std::vector<neat::LinkGene> removable_links;
    for (const auto& link : genome.get_links()) {
        if (essential_links.find(link.link_id) == essential_links.end()) {
            removable_links.push_back(link);
        }
    }

    if (removable_links.empty()) {
        return;
    }

    auto to_remove = rng.choose_random(removable_links);
    genome.get_links().erase(std::remove(genome.get_links().begin(), genome.get_links().end(), to_remove), genome.get_links().end());
    std::cout << "Removing link from " << to_remove.link_id.input_id << " to " << to_remove.link_id.output_id << " from genome " << genome.get_genome_id() << std::endl;
}

void Mutator::mutate_remove_link_fix(Genome &genome) {
    RNG rng;
    NeatConfig config;

    if (genome.get_links().empty()) {
        return; // Aucun lien à supprimer
    }

    // Identifier les liens essentiels (connectant entrées/sorties)
    std::unordered_set<neat::LinkId, neat::LinkIdHash> essential_links;

    // Liens connectant les neurones d'entrée aux autres
    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id < config.num_inputs) {
            for (const auto& link : genome.get_links()) {
                if (link.link_id.input_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    // Liens connectant les neurones à des sorties
    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id >= config.num_inputs && 
            neuron.neuron_id < config.num_inputs + config.num_outputs) {
            for (const auto& link : genome.get_links()) {
                if (link.link_id.output_id == neuron.neuron_id) {
                    essential_links.insert(link.link_id);
                }
            }
        }
    }

    // Filtrer les liens amovibles
    std::vector<neat::LinkGene> removable_links;
    for (const auto& link : genome.get_links()) {
        if (essential_links.find(link.link_id) == essential_links.end()) {
            removable_links.push_back(link);
        }
    }

    if (removable_links.empty()) {
        return; // Aucun lien amovible
    }

    // Choisir un lien à supprimer
    auto to_remove = rng.choose_random(removable_links);

    // Supprimer le lien choisi
    genome.get_links().erase(
        std::remove_if(
            genome.get_links().begin(),
            genome.get_links().end(),
            [&to_remove](const neat::LinkGene& link) {
                return link.link_id == to_remove.link_id;
            }),
        genome.get_links().end());

    std::cout << "Removing link from " << to_remove.link_id.input_id
              << " to " << to_remove.link_id.output_id
              << " from genome " << genome.get_genome_id() << std::endl;

    // Validation post-suppression
    validate_connectivity(genome);
}


void Mutator::mutate_add_neuron(Genome &genome) {
    RNG rng;

    if (genome.get_links().empty()) {
        return;
    }

    neat::LinkGene link_to_split = rng.choose_random(genome.get_links());
    link_to_split.is_enabled = false;

    genome.get_links().erase(std::remove_if(genome.get_links().begin(), genome.get_links().end(),
        [&link_to_split](const neat::LinkGene &link) {
            return link.link_id == link_to_split.link_id;
        }), genome.get_links().end());

    neat::NeuronMutator neuron_mutator;
    neat::NeuronGene new_neuron = neuron_mutator.new_neuron();
    new_neuron.neuron_id = genome.generate_next_neuron_id();
    genome.add_neuron(new_neuron);

    neat::LinkId link_id = link_to_split.link_id;
    double weight = link_to_split.weight;

    genome.add_link(neat::LinkGene{{link_id.input_id, new_neuron.neuron_id}, 1.0, true});
    genome.add_link(neat::LinkGene{{new_neuron.neuron_id, link_id.output_id}, weight, true});

    std::cout << "Adding neuron " << new_neuron.neuron_id << " to genome " << genome.get_genome_id() << std::endl;
}

void Mutator::mutate_add_neuron_fix(Genome &genome) {
    RNG rng;

    if (genome.get_links().empty()) {
        return;
    }

    neat::LinkGene link_to_split = rng.choose_random(genome.get_links());
    if (!link_to_split.is_enabled) {
        return; // Éviter de splitter un lien déjà désactivé
    }
    link_to_split.is_enabled = false;

    // Désactive le lien en le marquant (au lieu de l'effacer immédiatement)
    for (auto &link : genome.get_links()) {
        if (link.link_id == link_to_split.link_id) {
            link.is_enabled = false;
        }
    }

    // Crée un nouveau neurone
    neat::NeuronMutator neuron_mutator;
    neat::NeuronGene new_neuron = neuron_mutator.new_neuron();
    new_neuron.neuron_id = genome.generate_next_neuron_id();
    genome.add_neuron(new_neuron);

    // Ajoute deux nouveaux liens
    neat::LinkGene link1{{link_to_split.link_id.input_id, new_neuron.neuron_id}, 1.0, true};
    neat::LinkGene link2{{new_neuron.neuron_id, link_to_split.link_id.output_id}, link_to_split.weight, true};
    genome.add_link(link1);
    genome.add_link(link2);

    std::cout << "Adding neuron " << new_neuron.neuron_id << " to genome " << genome.get_genome_id() << std::endl;
}


void Mutator::mutate_remove_neuron(Genome &genome) {
    int hidden_neuron_count = std::count_if(genome.get_neurons().begin(), genome.get_neurons().end(), 
        [](const neat::NeuronGene &neuron) { 
            NeatConfig config;
            return neuron.is_hidden(neuron.neuron_id, config);
        });

    if (hidden_neuron_count < 2) {
        return;
    }

    RNG rng;
    auto neuron_it = choose_random_hidden(genome.get_neurons());

    genome.get_links().erase(std::remove_if(genome.get_links().begin(), genome.get_links().end(), 
        [neuron_it](const neat::LinkGene &link) {
            return link.has_neuron(*neuron_it);
        }),
        genome.get_links().end());

    std::cout << "Removing neuron " << neuron_it->neuron_id << " from genome " << genome.get_genome_id() << std::endl;

    genome.get_neurons().erase(neuron_it);
}

void Mutator::mutate_link_weight(Genome &genome, const NeatConfig &config, RNG &rng) {
    // Vérifie s'il y a des liens à muter
    if (genome.get_links().empty()) {
        return;
    }

    // Choisir un lien aléatoire
    int link_index = rng.next_int(0, genome.get_links().size() - 1);
    auto &link = genome.get_links()[link_index];

    // Appliquer la mutation si la probabilité le permet
    if (rng.next_double() < config.probability_mutate_link_weight) {
        std::cout << "Mutating link weight for genome " << genome.get_genome_id() << "for link"<< link.link_id.input_id << "to" << link.link_id.output_id << std::endl;
        link.weight = mutate_delta(link.weight);  // Muter le poids du lien
    }
}

void Mutator::mutate_remove_neuron_fix(Genome &genome) {
    // Compte les neurones cachés
    int hidden_neuron_count = std::count_if(genome.get_neurons().begin(), genome.get_neurons().end(), 
        [](const neat::NeuronGene &neuron) { 
            NeatConfig config;
            return neuron.is_hidden(neuron.neuron_id, config);
        });

    // Si on a moins de 2 neurones cachés, on ne peut pas en supprimer
    if (hidden_neuron_count < 2) {
        return;
    }

    // Sélectionne un neurone caché aléatoire
    std::vector<neat::NeuronGene>& neurons = genome.get_neurons();
    auto hidden_neurons = std::vector<std::vector<neat::NeuronGene>::iterator>();

    NeatConfig config;
    for (auto it = neurons.begin(); it != neurons.end(); ++it) {
        if (it->is_hidden(it->neuron_id, config)) {
            hidden_neurons.push_back(it);
        }
    }

    // Vérifie qu'il y a bien des neurones cachés
    if (hidden_neurons.empty()) {
        return; // Sécurité supplémentaire
    }

    RNG rng;
    auto neuron_it = hidden_neurons[rng.uniform(0, hidden_neurons.size() - 1)];

    // Supprime les liens associés au neurone
    auto& links = genome.get_links();
    links.erase(std::remove_if(links.begin(), links.end(), 
        [neuron_it](const neat::LinkGene &link) {
            return link.has_neuron(*neuron_it);
        }),
        links.end());

    std::cout << "Removing neuron " << neuron_it->neuron_id << " from genome " << genome.get_genome_id() << std::endl;

    // Supprime le neurone sélectionné
    neurons.erase(neuron_it);
}


void Mutator::mutate_neuron_bias(Genome &genome, const NeatConfig &config, RNG &rng) {
    // Vérifie s'il y a des neurones à muter
    if (genome.get_neurons().empty()) {
        return;
    }

    // Choisir un neurone aléatoire
    int neuron_index = rng.next_int(0, genome.get_neurons().size() - 1);
    auto &neuron = genome.get_neurons()[neuron_index];

    // Appliquer la mutation si la probabilité le permet
    if (rng.next_double() < config.probability_mutate_neuron_bias) {
        std::cout << "Mutating neuron bias for genome " << genome.get_genome_id() << "for neuron"<< neuron.neuron_id << std::endl;
        neuron.bias = mutate_delta(neuron.bias);  // Muter le biais du neurone
    }
}

void Mutator::validate_connectivity(const Genome &genome) {
    std::unordered_set<int> visited;
    std::queue<int> to_visit;

    // Ajouter tous les neurones d'entrée comme points de départ
    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id < genome.get_num_inputs()) {
            to_visit.push(neuron.neuron_id);
        }
    }

    // Parcourir le réseau
    while (!to_visit.empty()) {
        int current = to_visit.front();
        to_visit.pop();
        visited.insert(current);

        // Trouver les sorties connectées
        for (const auto& link : genome.get_links()) {
            if (link.link_id.input_id == current && 
                visited.find(link.link_id.output_id) == visited.end()) {
                to_visit.push(link.link_id.output_id);
            }
        }
    }

    // Vérifier que toutes les sorties sont accessibles
    for (const auto& neuron : genome.get_neurons()) {
        if (neuron.neuron_id >= genome.get_num_inputs() && 
            neuron.neuron_id < genome.get_num_inputs() + genome.get_num_outputs()) {
            if (visited.find(neuron.neuron_id) == visited.end()) {
                throw std::runtime_error("Network connectivity broken: output neuron " +
                                         std::to_string(neuron.neuron_id) + " is unreachable.");
            }
        }
    }
}



 int choose_random_input_or_hidden_neuron(const std::vector<neat::NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    NeatConfig config;

    for (const auto& neuron : neurons) {
        if (neuron.neuron_id < config.num_inputs || 
            neuron.neuron_id >= config.num_inputs + config.num_outputs) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }

    if (valid_neurons.empty()) {
        return -1;
    }

    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}

int choose_random_output_or_hidden_neuron(const std::vector<neat::NeuronGene>& neurons) {
    std::vector<int> valid_neurons;
    NeatConfig config;

    for (const auto& neuron : neurons) {
        if (neuron.neuron_id >= config.num_inputs && 
            neuron.neuron_id < config.num_inputs + config.num_outputs) {
            valid_neurons.push_back(neuron.neuron_id);
        }
    }
    if (valid_neurons.empty()) {
        return -1;
    }
    int random_index = std::rand() % valid_neurons.size();
    return valid_neurons[random_index];
}

std::vector<neat::NeuronGene>::const_iterator choose_random_hidden(std::vector<neat::NeuronGene>& neurons) {
    std::vector<std::vector<neat::NeuronGene>::const_iterator> hidden_neurons;
    NeatConfig config;

    for (auto it = neurons.begin(); it != neurons.end(); ++it) {
        if (it->neuron_id >= config.num_inputs + config.num_outputs) {
            hidden_neurons.push_back(it);
        }
    }

    if (hidden_neurons.empty()) {
        throw std::out_of_range("No hidden neurons available.");
    }

    RNG rng;
    return rng.choose_random(hidden_neurons);
}



bool would_create_cycle(const std::vector<neat::LinkGene>& links, int input_id, int output_id) {
    std::unordered_set<int> visited;

    std::function<bool(int)> dfs = [&](int current_id) {
        if (visited.find(current_id) != visited.end()) {
            return true;
        }
        visited.insert(current_id);

        for (const auto& link : links) {
            if (link.link_id.input_id == current_id) {
                if (link.link_id.output_id == input_id) {
                    return true;
                }
                if (dfs(link.link_id.output_id)) {
                    return true;
                }
            }
        }
        return false;
    };

    return dfs(output_id);
}

double new_value(){
    RNG rng;
    neat::DoubleConfig config;
    return neat::clamp(rng.next_gaussian(config.init_mean, config.init_stdev));
}

double mutate_delta(double value){
    RNG rng;
    neat::DoubleConfig config;
    double delta = neat::clamp( rng.next_gaussian(0, config.mutate_power));
    return neat::clamp (value + delta);
}

