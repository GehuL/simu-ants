#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <cmath>

class Activation {
public:
    // Fonction d'activation sigmoïde
    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    // Dérivée de la fonction sigmoïde
    static double sigmoid_derivative(double x) {
        return x * (1.0 - x);  // x est la sortie de la fonction sigmoïde
    }
};

#endif // ACTIVATION_H
