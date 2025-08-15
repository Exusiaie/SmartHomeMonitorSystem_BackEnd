#include "TcpConnection.hpp"
#include "EventLoop.hpp"      //实现文件，引入头文件
#include "InetAddress.hpp"
#include <sstream>
#include <sys/socket.h>
#include <cstring>
namespace wd{

//构造函数-------------------------------------------------------
TcpConnection::TcpConnection(int fd,EventLoop* loop)
:_sock(fd)
,_sockIO(fd)
,_localAddr(getLocalAddress(fd))
,_peerAddr(getPeerAddress(fd))
,_isShutdwonWrite(false)                    //是否关闭写，默认false
,_loop(loop)               
{

}

//析构函数--------------------------------------------------------
TcpConnection::~TcpConnection(){
    if(!_isShutdwonWrite){
        shutdown();           //?
    }
}

//接收数据--------------------
string TcpConnection::receive(){
    char buff[65536] = {0};
    int ret = _sockIO.readline(buff,sizeof(buff));
    return string(buff,ret);      //防止字符串内容中间有'\0'   ?
}

//读包，解析并存储---------------------------------------新
int TcpConnection::readPacket(Packet & packet){
    return _sockIO.readPacket(packet);         //SocketIO.readPacket->.readn->recv
}

//发送数据---------------------
void TcpConnection::send(const string & msg){
    if(msg.size() > 0){
        _sockIO.sendn(msg.c_str(),msg.size());
    }

}

//在循环中发送----------------
void TcpConnection::sendInLoop(const string & msg){
    if(_loop){
        _loop->runInLoop(std::bind(&TcpConnection::send,this,msg));
    }
}

//新加TLV功能-------------------------------------------------------
void TcpConnection::sendInLoop(const TLV & data){
    int tlvlen = 8 + data.length;               //tlv长度= 8(盐值) +数据的长度
    string msg;                                  //先构造空间
    msg.assign((const char*)&data,tlvlen);        //强制转换为char*,以便按字节，取指定长度
    sendInLoop(msg);                             //根据参数类型，再调用上一个sendInLoop
}

//判断是否关闭====================
bool TcpConnection::isClosed() const{
    char buff[20] = {0};
    return _sockIO.recvPeek(buff,sizeof(buff)) == 0;   //全部接收完了
}

//关闭============================
void TcpConnection::shutdown(){
    _sock.shutdownwrite();     //停止写，Socket.shutdownwrite->shutdown
}

//显示本地+对端：地址、端口信息==============
string TcpConnection::toString() const{
    std::ostringstream oss;
    oss << "tcp" << _localAddr.ip() << ":" << _localAddr.port()
        << "->"  << _peerAddr.ip()  << ":" << _peerAddr.port();

    return oss.str();
}

//处理新建连接回调函数?-------------------
void TcpConnection::handleNewConnectionCallback(){
    if(_onConnection){
        _onConnection(shared_from_this());
    }
}
    
//处理接收数据回调函数---------------------
void TcpConnection::handleMessageCallback(){
    if(_onMessage){
        _onMessage(shared_from_this());
    }
}

//处理关闭回调函数-------------------------
void TcpConnection::handleCloseCallback(){
    if(_onClose){
        _onClose(shared_from_this());
    }
}

//获取本地地址--------------------------------------------------
InetAddress TcpConnection::getLocalAddress(int fd){
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    socklen_t len = sizeof(addr);

    int ret = getsockname(_sock.fd(),(struct sockaddr*)&addr,&len);
    if(ret < 0){
        perror("getsockname");
    }

    return InetAddress(addr);
}

//获取对端地址---------------------------------------------------
InetAddress TcpConnection::getPeerAddress(int fd){
    struct sockaddr_in addr;               //创建结构体
    memset(&addr,0,sizeof(addr));          //清空内存，指定大小
    socklen_t len = sizeof(addr);  

    int ret = getpeername(_sock.fd(),(struct sockaddr*)&addr,&len);  //获取对端名字
    if(ret < 0){
        perror("getpeername");
    }

    return InetAddress(addr);
}

}

