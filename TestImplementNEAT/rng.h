#ifndef RNG_H
#define RNG_H

#include <random>

class RNG {
public:
    RNG() : gen(rd()) {}  // Initialisation du générateur

    // Méthode pour choisir entre deux valeurs en fonction d'une probabilité
    template <typename T>
    T choose(double probability, const T& a, const T& b) {
        std::uniform_real_distribution<> dis(0, 1);  // Génère un nombre entre 0 et 1
        return (dis(gen) < probability) ? a : b;     // Retourne a si la probabilité est respectée, sinon b
    }

private:
    std::random_device rd;  // Source d'entropie pour la génération aléatoire
    std::mt19937 gen;       // Générateur de nombres aléatoires basé sur Mersenne Twister
};

#endif // RNG_H
