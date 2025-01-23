#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <cmath>
#include <stdexcept>

class Activation
{
public:
    // Enum des types d'activation
    enum class Type
    {
        Sigmoid,
        Tanh,
        ReLU
    };

    /**
     * @brief Construit un objet Activation avec le type d'activation par défaut défini sur Sigmoid.
     */
    Activation() : activation_type(Type::Sigmoid) {}

    /**
     * @brief Construit un objet Activation avec le type d'activation spécifié.
     *
     * @param type Le type de fonction d'activation à utiliser.
     */
    Activation(Type type) : activation_type(type) {}

    /**
     * @brief Applique la fonction d'activation à la valeur d'entrée.
     *
     * Cette fonction applique la fonction d'activation spécifiée par le
     * membre `activation_type` à la valeur d'entrée `x`. Les fonctions
     * d'activation supportées sont Sigmoid et Tanh.
     *
     * @param x La valeur d'entrée à laquelle la fonction d'activation est appliquée.
     * @return Le résultat de l'application de la fonction d'activation à `x`.
     * @throws std::invalid_argument Si le type d'activation est inconnu.
     */
    double apply(double x) const
    {
        switch (activation_type)
        {
        case Type::Sigmoid:
            return sigmoid(x);
        case Type::Tanh:
            return tanh(x);
        case Type::ReLU:
            return std::max(0.0, x);    
        default:
            throw std::invalid_argument("Unknown activation type");
        }
    }

    Type get_type() const
    {
        return activation_type;
    }

private:
    Type activation_type;

    static double sigmoid(double x)
    {
        return 1.0 / (1.0 + std::exp(-x));
    }

    static double tanh(double x)
    {
        return std::tanh(x);
    }

    static double relu(double x)
    {
        return std::max(0.0, x);
    }
};
;

#endif // ACTIVATION_H
