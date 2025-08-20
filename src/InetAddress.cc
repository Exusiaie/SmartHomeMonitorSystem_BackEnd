#include "InetAddress.hpp"
#include <cstring>         //memset
#include <arpa/inet.h>     //inet_addr

namespace wd{

//地址             传参端口                 默认地址-------------------
InetAddress::InetAddress(unsigned short port,const string & ip){
    memset(&_addr,0,sizeof(_addr));
    _addr.sin_family = AF_INET;                      //IPv4
    _addr.sin_port = htons(port);                   //主机字节序->网络字节序
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());  //点分十进制->网络
}

//地址          传参    结构体-----------------------------------------？
InetAddress::InetAddress(const struct sockaddr_in & addr)
:_addr(addr)
{

}

//地址函数-----------------
string InetAddress::ip() const{
    return inet_ntoa(_addr.sin_addr);   //网络->点分十进制
}

//端口函数------------------
unsigned short InetAddress::port() const{
    return ntohs(_addr.sin_port);        //网络->主机字节序
}

}//namespace wd

