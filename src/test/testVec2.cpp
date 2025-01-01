#include "src/types.h"
#include <iostream>


using namespace simu;

int main(void)
{
    Vec2<float> v1{0.5f, 2.1f};
    Vec2<int> v2{3, 15};

    std::cout << v1 * 0.3 << std::endl;
    std::cout << v2 * 0.5 << std::endl;

    std::cout << v1 + v2 << std::endl;
    std::cout << v2 / 2 << std::endl;

    std::cout << (Vec2<double>{1.13, .52} + Vec2<float>{0.1, 0.001}) << std::endl;

    return 0;
}