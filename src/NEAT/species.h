#include <vector>

#include "Genome.h"


class Species {
public:
    int id;
    Genome representative; // Génome représentatif de l'espèce
    std::vector<std::shared_ptr<Genome>> members;

    Species(int id, const Genome &rep) : id(id), representative(rep) {}

    void add_member(const Genome &genome) {
        members.push_back(std::make_shared<Genome>(genome));
    }

    void clear_members() {
        members.clear();
    }
};
