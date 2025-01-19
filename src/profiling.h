#ifndef __PROFILING_H__
#define __PROFILING_H__

#include <unordered_map>
#include <string>
#include <stack>
#include <chrono>

namespace simu
{
    struct ProfileData
    {
        std::chrono::steady_clock::time_point lastTime;
        std::chrono::duration<double> elapsed;
        std::chrono::duration<double> cumul;
        std::chrono::duration<double> average;

        int sampleCnt;

        void reset() { cumul = std::chrono::duration<double>::zero(); sampleCnt = 0; };

        std::chrono::duration<double> calculAverage() { return (average = cumul / sampleCnt); };
        std::chrono::duration<double> elapsed_time() { return lastTime - std::chrono::steady_clock::now(); };
    };
    
    class Profiler
    {
        public: 
            Profiler();

            void begin(const std::string& name);
            void end();

            ProfileData* getProfile(const std::string& name);
            ProfileData* operator[](const std::string& name);

            void resetAll();

        private:
            typedef std::unordered_map<std::string, ProfileData> containerType;
            containerType mProfiles;
            std::stack<containerType::iterator> mStack;
    };
}


#endif 