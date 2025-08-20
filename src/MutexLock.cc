#include "MutexLock.hpp"
#include <pthread.h>

namespace wd{

//构造函数----------------
MutexLock::MutexLock(){
    pthread_mutex_init(&_mutex,NULL);  //互斥锁初始化
}

//析构函数---------------
MutexLock::~MutexLock(){
    pthread_mutex_destroy(&_mutex);   //互斥锁销毁
}

//上锁--------
void MutexLock::lock(){
    pthread_mutex_lock(&_mutex);     //上锁
}

//解锁-----------
void MutexLock::unlock(){
    pthread_mutex_unlock(&_mutex);
}

}
