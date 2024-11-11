#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <vector>
#include <unordered_map>
#include <cassert>
#include <optional>
#include <variant>
#include "Genome.h"
#include "ActivationFn.h"
#include "LayerManager.h"

struct NeuronInput
{
    int input_id;
    double weight;
};

struct Neuron
{
    int neuron_id;
    ActivationFn activation;
    double bias;
    std::vector<NeuronInput> inputs;
};

class FeedForwardNeuralNetwork
{
public:
    /**
     * @brief Constructeur par défaut pour la classe FeedForwardNeuralNetwork.
     *
     * Initialise un objet FeedForwardNeuralNetwork avec des vecteurs vides pour les identifiants d'entrée et de sortie, ainsi qu'une liste vide de neurones.
     */
    FeedForwardNeuralNetwork(std::vector<int> input_ids, std::vector<int> output_ids, std::vector<Neuron> neurons)
        : m_input_ids(std::move(input_ids)), m_output_ids(std::move(output_ids)), m_neurons(std::move(neurons)) {}

    /**
     * @brief Active le réseau de neurones avec un ensemble d'entrées.
     *
     * Cette méthode active le réseau de neurones avec un ensemble d'entrées spécifié.
     * Les valeurs d'entrée sont propagées à travers le réseau, et les valeurs de sortie
     * sont calculées en conséquence.
     *
     * @param inputs Un vecteur d'entrées à fournir au réseau de neurones.
     * @return Un vecteur de valeurs de sortie calculées par le réseau de neurones.
     *
     * @throws std::runtime_error Si l'id d'un neurone n'est pas trouvé dans le vecteur de valeurs.
     * @throws std::logic_error Si la taille des entrées ne correspond pas à la taille des neurones d'entrée.
     */
    std::vector<double> activate(const std::vector<double> &inputs);

    /**
     * @brief Crée un feedforward neural network à partir d'un génome.
     *
     * Cette fonction crée un feedforward neural network à partir d'un génome donné.
     * Elle parcourt les neurones et les connexions du génome pour construire le réseau
     * de neurones correspondant.
     *
     * @param genome Le génome à partir duquel construire le réseau de neurones.Il contient les informations sur les neurones et les connexions.
     * @return FeedForwardNeuralNetwork Le réseau de neurones créé à partir du génome.
     */
    static FeedForwardNeuralNetwork create_from_genome(const Genome &genome);

private:
    std::vector<int> m_input_ids;
    std::vector<int> m_output_ids;
    std::vector<Neuron> m_neurons;
};

/**
 * @brief Convertir un Activation en ActivationFn
 * @param activation Activation à convertir
 * @return ActivationFn correspondant à l'Activation
 */
ActivationFn convert_activation(const Activation &activation);

#endif // NEURALNETWORK_H
