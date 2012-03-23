#include "safequeue.h"
#include "waitfreequeue.h"

#include "scopeguard.h"
#include "enforce.h"
#include "simplelog.h"

#include <sstream>
#include <iomanip>

#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

long long GetTimeOfDay();
double CPU();
unsigned long CurrentThreadID();

template <typename T>
struct Player
{
    Player(T& p1, T& p2, size_t c) : ping(p1), pong(p2), count(c) {}
    void operator()()
    {
        try
        {
            CPU(); // initialize the CPU usage...
            for (std::size_t i = 0; i != count; ++i)
            {
                pong.Produce(ping.Consume());
            }
            LOG << "Thread " << CurrentThreadID() << ": " << std::setprecision(2) << 
                std::setiosflags(std::ios::fixed) << "CPU() = " << CPU() * 100 << "%";
        }
        catch(const std::exception& e)
        {
            LOG << e.what();        
        }
    }
    T& ping;
    T& pong;
    size_t count;
};

template <typename T>
void IntLoad(T& t, std::size_t count)
{
    for (std::size_t i = 0; i != count; ++i)
    {
        t.Produce(i);
    }
}

int GetParam(int argc, char* argv[], int index, int def)
{
    return argc > index ? boost::lexical_cast<int>(argv[index]) : def;
}

int main(int argc, char* argv[])
{
    try
    {
        typedef LockFreeQueue<int> IntQueue;
        //typedef WaitFreeQueue<int> IntQueue;
        //typedef SafeQueue<int> IntQueue;

        IntQueue q1, q2;
        const std::size_t BALLS = GetParam(argc, argv, 1, 100);
        LOG << "There are " << BALLS << " balls";
        IntLoad(q1, BALLS);
        const std::size_t SHOTS = GetParam(argc, argv, 2, 100000);
        LOG << "There are " << SHOTS << " shots";
        long long begin = GetTimeOfDay();
        Player<IntQueue> p1(q1, q2, SHOTS), p2(q2, q1, SHOTS);
        boost::thread t1(p1);
        boost::thread t2(p2);
        t1.join();
        t2.join();        
        long long end = GetTimeOfDay();
        LOG << "It took " << std::setprecision(2) << std::setiosflags(std::ios::fixed) << 
            (end - begin) / 1000000. << " seconds";        
        return 0;
    }
    catch(const std::exception& e)
    {
        LOG << e.what();        
    }
    return -1;
}
