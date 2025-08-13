#ifndef _Socket_H_ 
#define _Socket_H_ 

#include "Noncopyable.hpp"

namespace wd{

//套接字类，继承不可复制、赋值
class Socket : Noncopyable
{
public:
    //构造函数
    Socket();

    //禁止隐式类型转换
    explicit Socket(int);   

    //析构函数
    ~Socket();

    //调用套接字fd
    int fd() const{
        return _fd;
    }

    //停止写
    void shutdownwrite();

private:
    int _fd;     //套接字fd
};

}
#endif

