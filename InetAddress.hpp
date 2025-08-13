#ifndef _InetAddress_H_
#define _InetAddress_H_

#include <netinet/in.h>   //定义sockaddr_in
#include <string>
using std::string;

namespace wd{

class InetAddress{
public:
    //地址             传参端口                 默认地址-------------------
    InetAddress(unsigned short port,const string & ip = "0.0.0.0");

    //地址          传参    结构体-----------------------------------------？
    InetAddress(const struct sockaddr_in &);

    //地址函数-----------------
    string ip() const;

    //端口函数------------------
    unsigned short port() const;

    //结构体类型          获取地址函数----------------------
    struct sockaddr_in * getInetaddressPtr(){
        return &_addr;                          //?
    }

private:
    struct sockaddr_in _addr;    //结构体，地址
};

}
#endif

