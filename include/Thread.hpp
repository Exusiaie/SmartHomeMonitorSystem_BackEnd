#ifndef _Thread_H_ 
#define _Thread_H_

#include "Noncopyable.hpp"
#include <pthread.h>
#include <functional>   //function

namespace wd{
using ThreadCallback=std::function<void()>;

class Thread : Noncopyable
{
public:
    //参数为右值引用，表示要绑定右值?
    Thread(ThreadCallback && cb);

    //析构函数
    ~Thread(){}

    //启动一个子线程
    void start();    

    //等待一个子线程，运行结束
    void join();

private:
    static void * start_routine(void*);

private:
    pthread_t      _pthid;     //线程ID
    bool           _isRunning;  //线程是否正在运行
    ThreadCallback _cb;
};

}
#endif

