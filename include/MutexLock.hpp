#ifndef _MutexLock_H_ 
#define _MutexLock_H_

#include "Noncopyable.hpp"

#include <pthread.h>

namespace wd{

//互斥锁        继承不可复制、赋值-------------
class MutexLock : Noncopyable
{
public:
    MutexLock();
    ~MutexLock();

    void lock();
    void unlock();

    //获取线程互斥锁
    pthread_mutex_t * getMutexLockPtr(){
        return &_mutex;
    }

private:
    pthread_mutex_t _mutex;
};

//互斥锁，自动版本----------------------------------------
class MutexLockGuard
{
public:
    //构造函数，上锁-------------
    MutexLockGuard(MutexLock & m)
    :_mutex(m)
    {
        _mutex.lock();
    }

    //析构函数，解锁--------------
    ~MutexLockGuard(){
        _mutex.unlock();
    }

private:
    MutexLock & _mutex;
};

}
#endif

