#ifndef __WAITFREEQUEUE_H__
#define __WAITFREEQUEUE_H__

#include <lockfreequeue.h>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>

inline void AddMilliseconds(boost::xtime& t, int millisec)
{
    const int MILLI = 1000;
    const int NANO = 1000000000;
    
    if (millisec >= MILLI)
    {
        t.sec += millisec / MILLI;
        millisec = millisec % MILLI;
    }
    int nano = millisec * 1000000;
    t.nsec += nano;
    if (t.nsec >= NANO)
    {
        t.sec += 1;
        t.nsec -= NANO;
    }
}

template <typename T>
struct WaitFreeQueue
{
    WaitFreeQueue(int w = 1/*milliseconds*/) : wait_time(w) {}

    void Produce(const T& t)
    {
        queue.Produce(t);
        cond.notify_one();
    }

    bool Consume(T& t)
    {
        return queue.Consume(t);
    }

    T Consume()
    {
        T tmp;
        if (Consume(tmp))
            return tmp;
        // the queue is empty, try again (possible waiting...)
        boost::mutex::scoped_lock lock(mtx);
        while (!Consume(tmp))
        {
            boost::xtime t;
            boost::xtime_get(&t, boost::TIME_UTC);
            AddMilliseconds(t, wait_time);
            cond.timed_wait(lock, t);
        }
        return tmp;
    }
private:
    LockFreeQueue<T> queue;
    boost::condition cond;
    boost::mutex mtx;
    int wait_time;
};

#endif
