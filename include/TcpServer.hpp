#ifndef _TcpServer_H_ 
#define _TcpServer_H_

#include "Acceptor.hpp"     //建立连接
#include "EventLoop.hpp"    //事件循环
#include "TcpConnection.hpp" //TCP连接

namespace wd{

class TcpServer{
public:
    //构造函数-------------------------------------------------
    TcpServer(unsigned short port,const string & ip="0.0.0.0")
    :_acceptor(port,ip)
    ,_loop(_acceptor)
    {

    }

    //设置回调函数--------------传参---------------------------？
    void setAllCallbacks(TcpConnectionCallback && cb1,
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {
        _loop.setAllCallbacks(std::move(cb1),
                              std::move(cb2),
                              std::move(cb3));
    }

    //开始===================================
    void start(){
        _acceptor.ready();   //准备：地址、端口、bind、listen
        _loop.loop();        //loop.waitEpollFd(),根据情况，建立连接、处理事件、接收数据
    }

    //停止和开始函数，运行在不同的线程中======
    void stop(){          
        _loop.unloop();   //EventLoop.unloop,正在循环，设置为false
    }

private:
    Acceptor  _acceptor;   //建立新的fd
    EventLoop _loop;       //事件循环
};

}
#endif

