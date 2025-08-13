#ifndef _EventLoop_H_ 
#define _EventLoop_H_

#include "TcpConnection.hpp"
#include "MutexLock.hpp"
#include <sys/epoll.h>

#include <vector>
#include <map>
#include <memory>     //智能指针

using std::vector;
using std::map;
using std::shared_ptr;

namespace wd{

class TcpConnection;
using TcpConnectionPtr= shared_ptr<TcpConnection>;           //智能指针
using TcpConnectionCallback=function<void(TcpConnectionPtr)>;//回调函数？
using Function=function<void()>;                             //?

class Acceptor;

class EventLoop{
public:
    //构造函数---------------
    EventLoop(Acceptor&);

    //析构函数--------------
    ~EventLoop();

    //循环-------------------
    void loop();

    //停止循环---------------
    void unloop();

    //？
    void runInLoop(Functor && cb);

    //设置回调函数----------------------------------
    void setAllCallbacks(TcpConnectionCallback && cb1,
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {
        _onConnection = std::move(cb1);
        _onMessage = std::move(cb2);
        _onClose = std::move(cb3);
    }

private:
    //等待epoll-----------------------------
    void waitEpollFd();

    //处理新连接------------
    void handleNewConnection();

    //处理数据-----------
    void handleMessage(int);

    //创建epoll----------------------------
    int createEpollFd();

    //添加epoll读事件
    void addEpollReadEvent(int);

    //删除epoll读事件
    void delEpollReadEvent(int);

    //创建事件----------------------------
    int createEventfd();
    
    //处理读事件
    void handleReadEvent();

    //唤醒
    void wakeup();

    //正在等待处理的函数
    void doPendingFunctors();

private:
    int                        _epfd;
    int                        _eventfd;
    bool                       _isLooping;
    Acceptor &                 _acceptor;

    vector<struct epoll_event> _evtArr;
    map<int,TcpConnectionPtr>  _conns;

    //回调函数
    TcpConnectionCallback      _onConnection;
    TcpConnectionCallback      _onMessage;
    TcpConnectionCallback      _onClose;

    vector<Functor>            _pendingFunctors;  //等待处理的函数
    MutexLock                  _mutex;            //互斥锁
};
}
#endif

