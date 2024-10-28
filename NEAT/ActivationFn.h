#ifndef ACTIVATIONFN_H
#define ACTIVATIONFN_H

#include <variant>
#include <cmath>

// Fonctions d'activation possibles
struct Sigmoid {
    double operator()(double x) const {
        return 1.0 / (1.0 + std::exp(-x));
    }
};

struct ReLU {
    double operator()(double x) const {
        return std::max(0.0, x);
    }
};

struct Tanh {
    double operator()(double x) const {
        return std::tanh(x);
    }
};

// Alias de type pour les fonctions d'activation
using ActivationFn = std::variant<Sigmoid, ReLU, Tanh>;

#endif // ACTIVATIONFN_H
