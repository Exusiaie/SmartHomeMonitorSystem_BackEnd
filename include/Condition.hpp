#ifndef _Condition_H_ 
#define _Condition_H_ 

#include "Noncopyable.hpp"
#include <pthread.h>

namespace wd{

class MutexLock;              //互斥锁的前向声明，在头文件中

class Condition : Noncopyable
{
public:
    Condition(MutexLock &);
    ~Condition();

    void wait();
    void notifyOne();
    void notifyAll();

private:
    pthread_cond_t _cond;
    MutexLock &    _mutex;  //引用占据的空间=一个指针的大小   
};
}
#endif

