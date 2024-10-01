#ifndef RNG_H
#define RNG_H

#include <random>
#include <vector>

class RNG {
public:
    RNG() : gen(rd()) {}  // Initialisation du générateur

    // Méthode pour choisir entre deux valeurs avec une probabilité
    template <typename T>
    T choose(double probability, const T& a, const T& b) {
        std::uniform_real_distribution<> dis(0, 1);  // Génère un nombre entre 0 et 1
        return (dis(gen) < probability) ? a : b;     // Retourne a si la probabilité est respectée, sinon b
    }

    // Méthode pour choisir un élément aléatoire dans un vecteur
    template <typename T>
    T choose_random(const std::vector<T>& vec) {
        if (vec.empty()) {
            throw std::out_of_range("Cannot choose from an empty vector.");
        }
        std::uniform_int_distribution<> dis(0, vec.size() - 1);  // Distribution pour choisir un indice aléatoire
        return vec[dis(gen)];  // Retourner l'élément choisi aléatoirement
    }

        // Méthode pour choisir aléatoirement entre deux valeurs
    template <typename T>
    T choose(const T& a, const T& b) {
        std::uniform_int_distribution<> dis(0, 1);  // Génère un entier 0 ou 1
        return dis(gen) ? a : b;  // Retourne l'une des deux valeurs
    }

    // Méthode pour choisir un élément aléatoire parmi plusieurs options
    template <typename T>
    T choose_among(const std::initializer_list<T>& options) {
        if (options.size() == 0) {
            throw std::out_of_range("Cannot choose from an empty list.");
        }
        std::uniform_int_distribution<> dis(0, options.size() - 1);
        return *(std::begin(options) + dis(gen));  // Retourner un élément aléatoire
    }

    double next_gaussian(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(gen);  // Utilise le générateur de nombres aléatoires défini dans RNG
}

    double next_double() {
        std::uniform_real_distribution<> dis(0, 1);  // Distribution pour les nombres réels entre 0 et 1
        return dis(gen);  // Retourne un nombre aléatoire entre 0 et 1
    }

    template <typename T>
T* choose_random(const std::vector<T>& vec, int limit) {
    if (vec.empty() || limit <= 0) {
        throw std::out_of_range("Cannot choose from an empty vector or invalid limit.");
    }
    if (limit > vec.size()) {
        throw std::out_of_range("Limit is larger than the vector size.");
    }
    std::uniform_int_distribution<> dis(0, limit - 1);  // Distribution pour choisir un indice aléatoire entre 0 et limit-1
    return vec[dis(gen)];  // Retourner l'élément choisi aléatoirement
}

private:
    std::random_device rd;  // Source d'entropie pour la génération aléatoire
    std::mt19937 gen;       // Générateur de nombres aléatoires basé sur Mersenne Twister
};

#endif // RNG_H
