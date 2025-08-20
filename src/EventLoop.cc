#include "EventLoop.hpp"
#include "Acceptor.hpp"
#include "MutexLock.hpp"
#include "TcpConnection.hpp"
#include <cerrno>
#include <unistd.h>      //close
#include <cstring>      //memset
#include <sys/eventfd.h>  //event
namespace wd{

//构造函数---------------
EventLoop::EventLoop(Acceptor& acceptor)
:_epfd(createEpollFd())    //创建epoll的fd
,_eventfd(createEventfd()) //创建事件fd
,_isLooping(false)
,_acceptor(acceptor)
,_evtArr(1000)
{
    addEpollReadEvent(_acceptor.fd());  //监听收发数据
    addEpollReadEvent(_eventfd);        //监听事件
}

//析构函数--------------
EventLoop::~EventLoop(){
    close(_epfd);
    close(_eventfd);
}

//循环-------------------
void EventLoop::loop(){
    _isLooping = true;

    while(_isLooping){
        waitEpollFd();
    }
}

//停止循环---------------
void EventLoop::unloop(){
    _isLooping = false;
}

//？
void EventLoop::runInLoop(Functor && cb){
    {
        MutexLockGuard autolock(_mutex);  //自动锁
        _pendingFunctors.push_back(cb);   //放入准备函数，vector中
    }

    wakeup();   //通知IO线程发送数据
}

//等待epoll-----------------------------
void EventLoop::waitEpollFd(){
    int nready = epoll_wait(_epfd,_evtArr.data(),_evtArr.size(),5000);

    if(nready == -1 && errno == EINTR){   //软中断错误？
        return;
    }else if(nready == -1){               //错误
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }else if(nready == 0){                //超时
        printf("epoll timeout\n");
    }else{                                //正常
        for(int i = 0;i < nready;++i){
            int fd = _evtArr[i].data.fd;

            if(fd == _acceptor.fd()){    //当前fd和新建fd相同
                handleNewConnection();   //新建连接,
            }else if(fd == _eventfd){    //?
                handleReadEvent();       //读操作
                doPendingFunctors();     //进行消息的发送
            }else{  
                handleMessage(fd);      //已经建立好的连接
            }//if-else if
        }//for
    }//if-else if
}//void

//处理新连接------------
void EventLoop::handleNewConnection(){
    int netfd = _acceptor.accept();                      //获取新的连接
    addEpollReadEvent(netfd);                             //添加epoll监听

    TcpConnectionPtr conn(new TcpConnection(netfd,this));    //创建TcpConnection对象  
    conn->setAllCallbacks(_onConnection,_onMessage,_onClose); //设置三个回调函数

    _conns.insert(std::make_pair(netfd,conn));  //map中装入fd、TCP
    conn->handleNewConnectionCallback();       //调用连接建立时的函数对象？
}

//处理数据-----------
void EventLoop::handleMessage(int fd){
    auto iter = _conns.find(fd);      //从map中查找fd
    if(iter != _conns.end()){
        bool isClosed = iter->second->isClosed();  //tcp.isClosed

        if(isClosed){                           //如果已经断开
            iter->second->handleCloseCallback();   //tcp.回调函数
            delEpollReadEvent(fd);               //从epoll的监听红黑树上删除
            _conns.erase(fd);                   //从map中删除
        }else{  //未断开
            iter->second->handleMessageCallback();  //接收数据？ 
        }//if-else
    }//if
}//void 

//创建epoll----------------------------
int EventLoop::createEpollFd(){
    int fd = epoll_create1(0);    //创建实例

    if(fd < 0){
        perror("epoll_create1");
    }

    return fd;
}

//添加epoll读事件
void EventLoop::addEpollReadEvent(int fd){
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.data.fd = fd;
    ev.events = EPOLLIN;

    int ret = epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&ev);  //添加监听事件
    if(ret < 0){
        perror("epoll_ctl");
    }
}

//删除epoll读事件
void EventLoop::delEpollReadEvent(int fd){
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.data.fd = fd;
    int ret = epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,&ev);
    if(ret < 0){
        perror("epoll_ctl");
    }
}

//创建事件----------------------------
int EventLoop::createEventfd(){
    int fd = eventfd(0,0);
    if(fd < 0){
        perror("eventfd");
    }

    return fd;
}
    
//处理读事件
void EventLoop::handleReadEvent(){
    uint64_t howmany = 0;
    int ret = read(_eventfd,&howmany,sizeof(howmany));
    printf("\nhowmany:%1u\n",howmany);
    if(ret != sizeof(howmany)){       //如果数量不一致，出错了
        perror("read");
    }
}

//唤醒
void EventLoop::wakeup(){
    uint64_t one = 1;
    int ret = write(_eventfd,&one,sizeof(one));
    if(ret != sizeof(one)){
        perror("write");
    }
}

//正在等待处理的函数
void EventLoop::doPendingFunctors(){
    printf(">> doPendingFunctors\n");

    vector<Functor> tmp;

    {//使用语句块，减小加锁的范围
        MutexLockGuard autolock(_mutex);
        tmp.swap(_pendingFunctors);
    }

    for(auto & f : tmp){
        f();
    }
}

}

