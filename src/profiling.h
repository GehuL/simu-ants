#ifndef __PROFILING_H__
#define __PROFILING_H__

#include <unordered_map>
#include <string>
#include <stack>
#include <chrono>

namespace simu
{
    class ProfileData
    {
        public:

        static constexpr int SAMPLE_SIZE = 30;
        
        std::chrono::steady_clock::time_point lastTime;
        std::chrono::duration<double> samples[SAMPLE_SIZE];
        int sampleIdx;

        void reset();

        std::chrono::duration<double> calculAverage();
        std::chrono::duration<double> elapsedTime(); 
        double getFrequency();

        bool isOpen;
    };
    
    class Profiler
    {
        public:
            static constexpr bool SCOPED = true;
            static constexpr bool UNSCOPED = !SCOPED;

            Profiler();

            template<bool scoped = SCOPED>
            void begin(const std::string& name);
            
            
            void end();
            void end(const std::string& name);

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