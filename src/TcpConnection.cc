#include "TcpConnection.hpp"
#include "EventLoop.hpp"      //实现文件，引入头文件
#include "InetAddress.hpp"
#include "MyLogger.hpp"
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
using std::cout;
using std::endl;
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
        cout << "向客户端发送了数据： " << msg.c_str() << endl; 
        _sockIO.sendn(msg.c_str(),msg.size());
        cout << "向客户端发送了数据： " << msg.c_str() << endl; 
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
  int tlvlen = 8 + data.length; 
    
    // 创建足够大的缓冲区
    std::vector<char> buffer(tlvlen);
    
    // 手动序列化
    int net_type = htonl(data.type);
    int net_length = htonl(data.length);
    memcpy(buffer.data(), &net_type, sizeof(net_type));
    memcpy(buffer.data() + 4, &net_length, sizeof(net_length));
    memcpy(buffer.data() + 8, data.data.data(), data.length);
     
    if (data.length < 0 || data.length > 1024) {
        LogError("Invalid TLV length: %d", data.length);
        return;
    }
    // 安全发送
    sendInLoop(string(buffer.data(), tlvlen));
    //TLV netData = data;
    //netData.type = htonl(data.type);       // 主机->网络字节序
    //netData.length = htonl(data.length);   // 主机->网络字节序
    //int tlvlen = 8 + netData.length;               //tlv长度= 8(盐值) +数据的长度
    //string msg;                                  //先构造空间
    //msg.assign((const char*)&netData,tlvlen);        //强制转换为char*,以便按字节，取指定长度
    //sendInLoop(msg);                             //根据参数类型，再调用上一个sendInLoop
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

// 添加上下文API实现
void TcpConnection::setContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(_contextMutex);
    _contextMap[key] = value;
}

std::string TcpConnection::getContext(const std::string& key) const {
    std::lock_guard<std::mutex> lock(_contextMutex);
    auto it = _contextMap.find(key);
    return (it != _contextMap.end()) ? it->second : "";
}

void TcpConnection::clearContext(const std::string& key) {
    std::lock_guard<std::mutex> lock(_contextMutex);
    _contextMap.erase(key);
}

bool TcpConnection::hasContext(const std::string& key) const {
    std::lock_guard<std::mutex> lock(_contextMutex);
    return _contextMap.find(key) != _contextMap.end();
}
}

