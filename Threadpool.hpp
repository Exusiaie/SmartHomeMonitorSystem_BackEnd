#ifndef _Threadpool_H_
#define _Threadpool_H_ 

#include "Task.hpp"
#include "TaskQueue.hpp"
#include "Thread.hpp"

#include <vector>
#include <memory>       //智能指针
using std::vector;
using std::unique_ptr;

namespace wd{

class Threadpool{
public:
    //构造函数
    Threadpool(int,int);

    //开启线程的运行
    void start();

    //停止线程池的运行
    void stop();

    //Task函数对象，右值引用来表示
    void addTask(Task && task);
    
private:
    //子线程要执行的过程
    void doTask();

private:
    vector<unique_ptr<Thread>> _threads;
    int                        _threadNum;
    int                        _queSize;
    TaskQueue                  _taskque;
    bool                       _isExit;
};

}
#endif

