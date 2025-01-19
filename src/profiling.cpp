#include "profiling.h"
#include <utility>
#include <assert.h>

using namespace simu;

Profiler::Profiler() {}

void Profiler::begin(const std::string& name)
{
    mProfiles[name].lastTime = std::chrono::steady_clock::now();
    mStack.push(mProfiles.find(name));
}

void Profiler::end()
{
    assert(mStack.size() > 0 && "Aucun profiler à terminer");
    containerType::iterator it = mStack.top();
    auto now = std::chrono::steady_clock::now();
    it->second.elapsed = now - it->second.lastTime;
    it->second.cumul += it->second.elapsed;
    it->second.sampleCnt++;
    mStack.pop();
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