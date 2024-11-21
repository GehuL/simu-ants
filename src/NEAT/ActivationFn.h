#ifndef ACTIVATIONFN_H
#define ACTIVATIONFN_H

#include <variant>
#include <cmath>


/**
 * @struct Sigmoid
 * @brief Un foncteur qui implémente la fonction d'activation sigmoïde.
 *
 * La structure Sigmoid fournit un opérateur() surchargé qui prend une valeur de type double
 * en entrée et retourne le résultat de la fonction d'activation sigmoïde.
 *
 * La fonction sigmoïde est définie par :
 * \f[
 * \sigma(x) = \frac{1}{1 + e^{-x}}
 * \f]
 *
 * Cette fonction est couramment utilisée dans les réseaux de neurones pour introduire de la non-linéarité
 * dans le modèle.
 *
 * @param x La valeur d'entrée pour laquelle la fonction sigmoïde doit être calculée.
 * @return Le résultat de la fonction sigmoïde appliquée à la valeur d'entrée.
 */

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


using ActivationFn = std::variant<Sigmoid, ReLU, Tanh>;

#endif // ACTIVATIONFN_H
