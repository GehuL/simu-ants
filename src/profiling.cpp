#include "profiling.h"
#include <utility>
#include <assert.h>

using namespace simu;

void ProfileData::reset()
{
    for(int i = 1; i < ProfileData::SAMPLE_SIZE; i++)
        samples[i] = std::chrono::duration<double>::zero();
    sampleIdx = 0;
}

std::chrono::duration<double> ProfileData::calculAverage()
{
    std::chrono::duration<double> cumul = samples[0];
    for(int i = 1; i < ProfileData::SAMPLE_SIZE; i++)
        cumul += samples[i];
    return cumul / SAMPLE_SIZE;
}

std::chrono::duration<double> ProfileData::elapsedTime()
{
    return std::chrono::steady_clock::now() - lastTime;
}

double ProfileData::getFrequency()
{
    return 1.0 / calculAverage().count();
}

Profiler::Profiler() {}

template<> void Profiler::begin<true>(const std::string& name)
{
    ProfileData* p = &mProfiles[name];
    assert(!p->isOpen && "Profiler already opened");        
    p->lastTime = std::chrono::steady_clock::now();
    p->isOpen = true;
    mStack.push(mProfiles.find(name));
}

template<> void Profiler::begin<false>(const std::string& name)
{
    ProfileData* p = &mProfiles[name];
    assert(!p->isOpen && "Profiler already opened");      
    mProfiles[name].lastTime = std::chrono::steady_clock::now();
    p->isOpen = true;
}

void Profiler::end()
{
    assert(mStack.size() > 0 && "Aucun profiler à terminer");

    containerType::iterator it = mStack.top();
    assert(it->second.isOpen && "Profiler out of scope");

    auto now = std::chrono::steady_clock::now();
    
    ProfileData* data = &it->second;
    data->isOpen = false;

    data->samples[data->sampleIdx] = now - it->second.lastTime;
    data->sampleIdx = (data->sampleIdx + 1) % ProfileData::SAMPLE_SIZE; 

    mStack.pop();
}

void Profiler::end(const std::string& name)
{
    if(mProfiles.find(name) != mProfiles.end())
    {
        ProfileData* data = &mProfiles[name];
        assert(data->isOpen && "Profiler out of scope (already closed)");

        auto now = std::chrono::steady_clock::now();
        data->samples[data->sampleIdx] = now - data->lastTime;
        data->sampleIdx = (data->sampleIdx + 1) % ProfileData::SAMPLE_SIZE; 
        data->isOpen = false;
    }else
    {
        mProfiles[name] = ProfileData();
    }
}

void Profiler::resetAll()
{
    for(auto it  = mProfiles.begin(); it != mProfiles.end(); it++)
        it->second.reset();
}

ProfileData* Profiler::getProfile(const std::string& name)
{
    assert(mProfiles.find(name) != mProfiles.end() && "Aucun profiler de ce nom");
    return &mProfiles.at(name);
}

ProfileData* Profiler::operator[](const std::string& name)
{
    return &mProfiles.at(name);
}