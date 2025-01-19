#include "../src/profiling.h"
#include <chrono>
#include <thread>
#include <iostream>

int main(void)
{
    using namespace std::chrono_literals;

    simu::Profiler profiler;
    profiler.begin("test");

    std::this_thread::sleep_for(1200ms);

    profiler.end();

    profiler["test"]->calculAverage();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(profiler["test"]->elapsed).count() << "ms" << std::endl; 
}