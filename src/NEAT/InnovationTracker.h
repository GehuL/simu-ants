#include "Neat.h"
#include <unordered_map>


class InnovationTracker {
public:
    static int next_innovation_number; 

    static int get_new_innovation() {
        return next_innovation_number++;
    }
};


int InnovationTracker::next_innovation_number = 0;

