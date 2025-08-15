#ifndef _TcpConnection_H_ 
#define _TcpConnection_H_

#include "Socket.hpp"
#include "SocketIO.hpp"
#include "InetAddress.hpp"

#include <memory>
#include <functional>
using std::shared_ptr;
using std::function;

namespace wd{

class EventLoop;  //前向声明

class TcpConnection;     //?
using TcpConnectionPtr=shared_ptr<TcpConnection>;
using TcpConnectionCallback=function<void(TcpConnectionPtr)>;

//                  继承辅助类          使用这个函数
class TcpConnection :public std::enable_shared_from_this<TcpConnection>
{
public:
    //构造函数
    TcpConnection(int fd,EventLoop* loop);

    //析构函数
    ~TcpConnection();

    //回调函数
    void setAllCallbacks(const TcpConnectionCallback & cb1,
                         const TcpConnectionCallback & cb2,
                         const TcpConnectionCallback & cb3)
    {
        _onConnection = cb1;
        _onMessage = cb2;
        _onClose = cb3;
    }

    //接收数据
    string receive();

    //读包----------------------------------------------------8.13
    int readPacket(Packet & packet);

    //发送数据
    void send(const string & msg);

    //在循环中发送
    void sendInLoop(const string & msg);

    //用于处理TLV 解析------------------------------------------
    void sendInLoop(const TLV & data);           

    //判断是否关闭
    bool isClosed() const;

    //关闭
    void shutdown();

    //显示地址、端口信息
    string toString() const;

    //处理新建连接回调函数
    void handleNewConnectionCallback();
    
    //处理接收数据回调函数
    void handleMessageCallback();

    //处理关闭回调函数
    void handleCloseCallback();

private:
    //获取本地地址
    InetAddress getLocalAddress(int fd);

    //获取对端地址
    InetAddress getPeerAddress(int fd);

private:
    Socket      _sock;
    SocketIO    _sockIO;

    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool        _isShutdwonWrite;    //是否要主动关闭连接  
    EventLoop * _loop;

    //回调函数
    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

}
#endif

