#ifndef _SocketIO_H_
#define _SocketIO_H_
#include "Message.hpp"

namespace wd{

class SocketIO{
public:
    //构造函数，禁止隐式类型转换----------
    explicit SocketIO(int fd)
    :_fd(fd)
    {

    }

    //读数据-------------------------
    int readn(char * buff,int len);

    //发送数据------------------------
    int sendn(const char * buff,int len);     //=writen

    //读取一行，              最大接收长度
    int readline(char * buff,int maxlen);

    int readPacket(Packet & packet);

    //接收
    int recvPeek(char * buff,int maxlen) const;

private:
    int _fd;
};

}
#endif

