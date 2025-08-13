#include "Acceptor.hpp"
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <tuple>

namespace wd{

//构造函数------------------------------------------------
Acceptor::Acceptor(unsigned short port,const string & ip)
:_addr(port,ip)
,_sock()
{

}
    
//准备函数集合
void Acceptor::ready(){
    setReuseAddr(true);
    setReusePort(true);
    bind();
    listen();
}

//建立新套接字--------------------------------------
int Acceptor::accept(){
    int netfd = ::accept(_sock.fd(),nullptr,nullptr);
    if(netfd < 0){
        perror("accept");
    }

    return netfd;
}

//重用地址--------------------------
void Acceptor::setReuseAddr(bool on){
    int value = on;     //          套接字层   重新绑定即将关闭的地址
    int ret = setsockopt(_sock.fd(),SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value));
    if(ret < 0){
        perror("setsockopt");
    }
}

//重用端口--------------------------
void Acceptor::setReusePort(bool on){
    int value = on;
    int ret = setsockopt(_sock.fd(),SOL_SOCKET,SO_REUSEPORT,&value,sizeof(value));
    if(ret < 0){
        perror("setsockopt");
    }
}

//绑定----------------
void Acceptor::bind(){  //        转换地址
    int ret = ::bind(_sock.fd(),(const struct sockaddr*)_addr.getInetaddressPtr(),sizeof(_addr));
}

//监听-----------------
void Acceptor::listen(){
    int ret = ::listen(_sock.fd(),10000);
    if(ret < 0){
        perror("listen");
    }
}  

}

