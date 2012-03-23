#include "enforce.h"
#include "scopeguard.h"

#include <map>

#include <sys/time.h>
#include <sys/resource.h>

#include <boost/thread.hpp>

long long GetTimeOfDay()
{
    timeval now = {0};    
    ENFORCE(::gettimeofday(&now, 0) == 0);
    long long ret = 1000000;
    ret *= now.tv_sec;
    ret += now.tv_usec;
    return ret;
}

struct RUsage
{
    RUsage();
    timeval t;
    rusage u;
};

RUsage::RUsage()
{
    ENFORCE(gettimeofday(&t, 0) == 0);
    ENFORCE(getrusage(RUSAGE_SELF, &u) == 0);
}

int CPUCount()
{
    FILE* pFile = fopen("/proc/stat", "r");
    if (!pFile)
        return -1;
    ON_BLOCK_EXIT(&fclose, pFile);
    char buffer[BUFSIZ + 1] = {0};
    int i = 0; 
    while (fgets(buffer, BUFSIZ, pFile)) 
    {
        if (strstr(buffer, "cpu"))
            ++i;
    }
    --i;
    ENFORCE(i > 0);
    return i;
}

long long TimeDiff(const timeval& t1, const timeval& t2)
{
    return (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
}

double CPU(const RUsage& u1, const RUsage& u2)
{

    static double cpuCount = CPUCount();
    return (1. * TimeDiff(u2.u.ru_utime, u1.u.ru_utime) + TimeDiff(u2.u.ru_stime, u1.u.ru_stime)) / 
        TimeDiff(u2.t, u1.t) / cpuCount;
}

unsigned long CurrentThreadID()
{
    return pthread_self();
}

double CPU()
{
    static boost::mutex mtx;
    boost::mutex::scoped_lock l(mtx);
    typedef std::map<unsigned long, RUsage> TUsages;
    static TUsages usages;
    RUsage& u1 = usages[CurrentThreadID()]; // insert if not found
    RUsage u2;
    double cpu = CPU(u1, u2);
    u1 = u2;
    if (cpu <= 0)
        return 0;
    if (cpu >= 1)
        return 1;
    return cpu;
}

