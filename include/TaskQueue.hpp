#ifndef _TaskQueue_H_ 
#define _TaskQueue_H_

#include "Task.hpp"       //任务
#include "MutexLock.hpp"  //互斥锁
#include "Condition.hpp"  //条件变量

#include <queue>
using std::queue;

namespace wd{
using ElemType = Task;  //?

class TaskQueue{
public:
    TaskQueue(int);

    bool empty() const;
    bool full() const;

    void push(ElemType);
    ElemType pop();

    void wakeup();     //唤醒

private:
    queue<ElemType> _que;     //队列
    int             _queSize; //队列大小

    MutexLock       _mutex;   //互斥锁

    Condition       _notFull;  //不满
    Condition       _notEmpty; //不为空

    bool            _flag;     //标志
};

}
#endif

