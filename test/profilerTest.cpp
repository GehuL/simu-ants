#include "../src/profiling.h"
#include <chrono>
#include <thread>
#include <iostream>

int main(void)
{
    using namespace std::chrono_literals;

    simu::Profiler profiler;

    for(int i = 0; i < simu::ProfileData::SAMPLE_SIZE; i++)
    {
        profiler.end("test");
        profiler.begin<simu::Profiler::UNSCOPED>("test");
        profiler.begin("50");
        std::this_thread::sleep_for(10ms);
        profiler.end();
    }

    std::cout << profiler["test"]->calculAverage().count() << std::endl;
    std::cout << profiler["test"]->getFrequency() << std::endl;

    std::cout << profiler["50"]->calculAverage().count() << std::endl;
    std::cout << profiler["50"]->getFrequency() << std::endl;
}