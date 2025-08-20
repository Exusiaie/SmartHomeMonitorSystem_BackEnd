#include "Socket.hpp"    //用于生成套接字
#include <sys/socket.h>  //socket
#include <stdio.h>       //perror
#include <unistd.h>      //close

namespace wd{

//构造函数------------------------
Socket::Socket(){
    _fd = socket(AF_INET,SOCK_STREAM,0); //IPv4,TCP,生成fd
    if(_fd < 0){
        perror("socket");
    }
}

//禁止隐式类型转换,带参数型--------
Socket::Socket(int fd)
:_fd(fd)
{

}   

//析构函数---------------------------
Socket::~Socket(){
    close(_fd);    //析构同时，关闭fd
}

//主动断开连接时，调用-----------------------------------------------
void Socket::shutdownwrite(){
    shutdown(_fd,SHUT_WR);   //不会立即关闭套接字fd,优雅终止，只关闭写端
}

}

