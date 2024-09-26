#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <cmath>
#include <stdexcept>

class Activation {
public:
    // Enum des types d'activation
    enum class Type {
        Sigmoid,
        Tanh
    };

    // Constructeur par défaut (utilise Sigmoid par défaut)
    Activation() : activation_type(Type::Sigmoid) {}

    // Constructeur qui initialise avec un type spécifique
    Activation(Type type) : activation_type(type) {}

    // Appliquer la fonction d'activation en fonction du type
    double apply(double x) const {
        switch (activation_type) {
            case Type::Sigmoid:
                return sigmoid(x);
            case Type::Tanh:
                return tanh(x);
            default:
                throw std::invalid_argument("Unknown activation type");
        }
    }

    Type get_type() const {
        return activation_type;
    }

private:
    Type activation_type;

    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    static double tanh(double x) {
        return std::tanh(x);
    }
};
;

#endif // ACTIVATION_H
