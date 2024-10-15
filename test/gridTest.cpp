#include "../src/tiles.h"
#include <fstream>

int main()
{
    simu::Grid grid(513, 5);

    std::ofstream f("test.txt", std::ios_base::out);
    f << grid.compressGrid();
    f.close();

    std::ifstream in("test.txt", std::ios_base::in);
    std::string data;
    data << in;
    in.close();

    grid.decompressGrid(data);
}