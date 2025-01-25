#include "../src/tiles.h"
#include <fstream>

int main()
{
    simu::Grid grid(100, 5);
    std::string data;

    std::ofstream f("test.txt", std::ios_base::out);
    simu::compressGrid(grid, data);
    f << data;
    f.close();

    std::ifstream in("test.txt", std::ios_base::in);
    std::getline(in, data);
    in.close();

    simu::decompressGrid(grid, data, 5);
}