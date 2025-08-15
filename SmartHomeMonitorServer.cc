#include "SmartHomeMonitorServer.hpp"
#include "BusinessService.hpp"         //使用用户验证第一部分类
#include "Message.hpp"                 //使用packet结构体，存储TLV内容
#include "MyLogger.hpp"

#include <iostream>
using std::cout;
using std::endl;

//构造函数------------------------------------------
SmartHomeMonitorServer::SmartHomeMonitorServer(int threadNum,int taskSize,unsigned short port,const string & ip)
    :_threadpool(threadNum,taskSize)         //线程池初始化，设定线程数量，线程大小
    ,_server(port,ip)                        //服务器端口，IP
{
    using namespace std::placeholders;

    //设置回调函数
    _server.setAllCallbacks(std::bind(&SmartHomeMonitorServer::onConnection,this,_1),
                            std::bind(&SmartHomeMonitorServer::onMessage,this,_1),
                            std::bind(&SmartHomeMonitorServer::onClose,this,_1));
}

    //~SmartHomeMonitorServer() {}    //为什么不需要析构函数？

//开始函数----------------------------
void SmartHomeMonitorServer::start(){
    _threadpool.start();             //线程池打开,创建子线程对象，vector存储子线程，都运行
    _server.start();                 //TcpServer.start->Acceptor.ready+ EventLoop.loop.waitEpollFd()
}
    
//停止函数-----------------------------
void SmartHomeMonitorServer::stop(){
    _server.stop();                  // TcpServer.stop-> EventLoop.unloop是否正在循环标志false               
    _threadpool.stop();
}

//进行连接？
void SmartHomeMonitorServer::onConnection(wd::TcpConnectionPtr conn){
    LogInfo("tcp %s has connected.\n",conn->toString().c_str());      //显示端口、地址信息
}

//处理信息
void SmartHomeMonitorServer::onMessage(wd::TcpConnectionPtr conn){
    cout << "onMessage..." << endl;

    wd::Packet packet;                               //解析TLV格式之后的消息，放在packet中
    int ret = conn->readPacket(packet);               //TcpConnection.readPacket->SocketIO.readPacket
                                                      
    cout << "read:" << ret << "bytes.\n";             //将返回的字节数，打印出来
    cout << "packet.type:" <<packet.type << endl      //解析之后的type
         << "packet.length:" <<packet.length << endl  //length
         << "packet.msg:" <<packet.msg << endl;       //value

    //根据类型，判断密码验证阶段
    switch(packet.type)
    {
    case wd::TASK_TYPE_LOGIN_SECTION1:   //如果是第一阶段
        {
            UserLoginSection1 userLogin1(conn,packet);  //传入的conn,已接收的包
            //线程池异步执行，添加任务，绑定一阶段的process函数，放入任务队列
            _threadpool.addTask(std::bind(&UserLoginSection1::process,userLogin1));//可调用对象
        }
        break;
    case wd::TASK_TYPE_LOGIN_SECTION2:   //如果是第二阶段
        {
            //待写
        }
        break;
    }
}

void SmartHomeMonitorServer::onClose(wd::TcpConnectionPtr conn){
    LogInfo("tcp %s has closed.\n",conn->toString().c__str());
}


