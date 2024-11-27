// LayerManager.h
#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include <vector>
#include <unordered_set>
#include "Neat.h"

class LayerManager
{
public:
    /**
     * @brief Identifie les couches de neurones en fonction des liens fournis.
     *
     * Cette fonction organise les neurones en couches à partir des neurones d’entrée,
     * puis en ajoutant progressivement des couches de neurones sur la base des liens fournis,
     * en s’assurant que les neurones d’une couche sont tous connectés aux neurones de la couche précédente.
     *
     * @param inputs Un vecteur d'entiers représentant les ID des neurones d'entrée.
     * @param outputs Un vecteur d'entiers représentant les ID des neurones de sortie.
     * @param links Un vecteur de neat::LinkGene représentant les liens entre les neurones.
     *
     * @return Vecteur de vecteurs d’entiers, où chaque vecteur interne représente une couche d’identificateurs neuronaux.
     *
     * @note Cette fonction suppose que les neurones d'entrée et de sortie sont correctement connectés.
     */
    static std::vector<std::vector<int>> organize_layers(
        const std::vector<int> &inputs,
        const std::vector<int> &outputs,
        const std::vector<neat::LinkGene> &links);

    /**
     * @brief Trie les neurones par couche en fonction des liens fournis.
     *
     * Cette fonction trie les neurones par couche en fonction des liens fournis, en s’assurant que
     * les neurones d’une couche sont tous connectés aux neurones de la couche précédente.
     *
     * @param layer Un vecteur d'entiers représentant les ID des neurones d'une couche.
     * @param links Un vecteur de neat::LinkGene représentant les liens entre les neurones.
     *
     * @return Vecteur d'entiers représentant les ID des neurones triés par couche.
     */
    static std::vector<int> sort_by_layer(
        const std::vector<int> &layer,
        const std::vector<neat::LinkGene> &links);

private:
};

#endif // LAYER_MANAGER_H
