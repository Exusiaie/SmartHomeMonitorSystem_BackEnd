#include "Threadpool.hpp"
//#include <unistd.h>
//#include <stdio.h>

namespace wd{

class Threadpool{

//构造函数
Threadpool::Threadpool(int threadNum,int queSize)
:_threads()           //vector的无参构造函数，没有开辟空间？
,_threadNum(threadNum)
,_queSize(queSize)
,_taskque(_queSize)
,_isExit(false)
{
    _threads.reserve(_threadNum);  //提前准备好空间,但还没有元素，size()为0
}

//开启线程的运行---------------------------------------------------------------
void Threadpool::start(){
    //创建线程------------------------
    for(int i = 0;i < _threadNum;++i){
        //创建N个子线程对象，线程池的doTask方法注册给线程对象
        unique_ptr<Thread> up(new Thread(std::bind(&Threadpool::doTask,this)));
        //up是左值，转换成右值，存入vector中
        _threads.push_back(std::move(up));
    }

    //让每一个子线程运行起来----------
    for(auto & pthread : _threads){
        pthread->start();
    }
}

//停止线程池的运行--------------------------------------------------------------
void Threadpool::stop(){
    //任务队列非空，不退出，等待任务执行完毕
    while(!_taskque.empty()){
        sleep(1);
    }

    _isExit = true;   //修改退出的标志位
    _taskque.wakeup(); //通知所有的子线程，要从wait中醒来？
    printf("\nthreadpool is starting join sub thread\n");

    //要回收每一个子线程
    for(auto & pthread : _threads){
        pthread->join();
    }
}

//Task函数对象，右值引用来表示
void Threadpool::addTask(Task && task){
    if(task){
        _taskque.push(std::move(task)); //存入任务队列
    }
}
    
//子线程要执行的过程
void Threadpool::doTask(){
    while(!_isExit){
        Task task = _taskque.pop();

        if(task){
            task();   //执行任务
                      //bug;子线程的任务执行的速度过快
                      //stop方法，还没来得及将_isExit设置为true
                      //子线程，依然运行，并进入下一轮循环，阻塞在pop方法上？
        }//if
    }//while
}

}

