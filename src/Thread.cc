#include "Thread.hpp"
#include <stdio.h>
#include <string.h>

namespace wd{

//参数为右值引用，表示要绑定右值?
Thread::Thread(ThreadCallback && cb)
:_pthid(0)
,_isRunning(false)
,_cb(std::move(cb))
{

}

//启动一个子线程
void Thread::start(){
    if(!_isRunning){
        int ret = pthread_create(&_pthid,nullptr,start_routine,this);
        if(ret != 0){
            fprintf(stderr,"%s",strerror(ret));
            return;
        }

        _isRunning = true;
    }
}    

//等待一个子线程，运行结束
void Thread::join(){
    if(_isRunning){
        pthread_join(_pthid,nullptr);

        _isRunning = false;         //更改子线程的状态
    }
}

//静态成员函数，没有this指针，无法直接获取到Thread的对象，通过线程入口函数的参数来传递？
void * Thread::start_routine(void* arg){
    //线程入口函数内部，必须要调用run方法，代表线程要执行的任务？
    Thread * pthread = static_cast<Thread*>(arg);
    if(pthread){
        pthread->_cb();    //基类指针，调用纯虚函数
    }

    return nullptr;
}

}
