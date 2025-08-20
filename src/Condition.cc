#include "Condition.hpp"
#include <pthread.h>
#include "MutexLock.hpp"   //实现文件中，加载MutexLock头文件
                
namespace wd{

//构造函数------------------------
Condition::Condition(MutexLock & m)
:_mutex(m)      //互斥锁初始化
{
    pthread_cond_init(&_cond,NULL);  //初始化
}

//析构函数------------------------
Condition:: ~Condition(){
    pthread_cond_destroy(&_cond);    //销毁
}

//等待----------------
void Condition::wait(){
    pthread_cond_wait(&_cond,_mutex.getMutexLockPtr());
}

//唤醒一个-----------------------
void Condition::notifyOne(){
    pthread_cond_signal(&_cond);
}

//唤醒所有------------------------
void Condition::notifyAll(){
    pthread_cond_broadcast(&_cond);
}

}
