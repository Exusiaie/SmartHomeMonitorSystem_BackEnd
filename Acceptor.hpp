#ifndef _Acceptor_H_
#define _Acceptor_H_

#include "InetAddress.hpp"
#include "Socket.hpp"

namespace wd{

class Acceptor{
public:
    //构造函数
    Acceptor(unsigned short port,const string & ip = "0.0.0.0");
    
    //准备函数集合
    void ready();

    //建立新套接字
    int accept();

    //调用此函数，会调用套接字fd
    int fd() const{
        return _sock.fd();
    }

private:
    //重用地址
    void setReuseAddr(bool);

    //重用端口
    void setReusePort(bool);

    //绑定
    void bind();

    //监听
    void listen();  

private:
    InetAddress _addr;   //地址
    Socket      _sock;   //套接字，不可复制，所以Acceptor对象也不能复制    
};

}
#endif

