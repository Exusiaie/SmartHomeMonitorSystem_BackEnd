#include "TaskQueue.hpp"
#include "MutexLock.hpp"

namespace wd{

//构造函数-------------------------------------------------------------------
TaskQueue::TaskQueue(int queSize)
:_que()
,_queSize(queSize)
,_mutex()
,_notFull(_mutex)
,_notEmpty(_mutex)
,_flag(true)
{

}

//空-----------------------------
bool TaskQueue::empty() const{
    return _que.size() == 0;  //大小=0
}

//满-----------------------------
bool TaskQueue::full() const{
    return _que.size() ==  _queSize; //大小=设置的最大值

}

//存入任务队列---------------------------------------------
void TaskQueue::push(ElemType e){
    MutexLockGuard autolock(_mutex);   //自动锁

    //满的时候,放不进去
    while(full()){
        _notFull.wait();    //不满，等一会再使用
    }

    _que.push(e);           //队列，存入e

    _notEmpty.notifyOne();  //不空，通知消费者线程，取数据
}

//出队-----------------------------------------------------
ElemType TaskQueue::pop(){
    ElemType tmp;
    MutexLockGuard autolock(_mutex);

    while(_flag && empty()){  //空，并且标志位？
        _notEmpty.wait();     //不空就要等一下再用
    }

    if(_flag){
        tmp = _que.front();
        _que.pop();            //出队
        _notFull.notifyOne();  //因为出去一个，所以不满了，通知生产者线程，放数据
    }else{
        tmp = nullptr;
    }

    return tmp;
}

//唤醒-------------------
void TaskQueue::wakeup(){
    _flag = false;          //标志置为false
    _notEmpty.notifyAll();  //不空，全部唤醒
}

}

