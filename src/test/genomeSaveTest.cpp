#include <fstream>
#include "../NEAT/genome.h"
#include "../external/json.hpp"
#include "../NEAT/rng.h"

struct Test
{
    int i;
    float a;
    std::string s;
};

void to_json(nlohmann::json& j, const Test& t)
{
    j = nlohmann::json{{"i", t.i}, {"a", t.a}, {"s", t.s}};
}

int main(void)
{
    using json = nlohmann::json;
    using namespace neat;

    RNG rng;
    Genome genome = Genome::create_genome(0, 3, 1, 3, rng);

    json j = genome;
    std::ofstream ofs("genome.json");
    ofs << j;
    ofs.flush();
    ofs.close(); 

    std::ifstream ifs("genome.json");
    json j2 = json::parse(ifs);

    Genome genome2 = j2.get<Genome>();

    return 0;
}