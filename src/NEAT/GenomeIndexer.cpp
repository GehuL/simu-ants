#include "GenomeIndexer.h"

GenomeIndexer::GenomeIndexer() : current_index(0) {}

int GenomeIndexer::next() {
    return current_index++;  // Renvoie l'index actuel et l'incrémente
}
