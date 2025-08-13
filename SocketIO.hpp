#ifndef _SocketIO_H_
#define _SocketIO_H_

namespace wd{

class SocketIO{
public:
    //构造函数----------
    SocketIO(int fd)
    :_fd(fd)
    {

    }

    //读数据-------------------------
    int readn(char * buff,int len);

    //发送数据------------------------
    int sendn(const char * buff,int len);

    //读取一行，              最大接收长度
    int readline(char * buff,int maxlen);

    //接收
    int recvPeek(char * buff,int maxlen) const;

private:
    int _fd;
};

}
#endif

