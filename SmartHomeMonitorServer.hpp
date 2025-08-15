#ifndef _SmartHomeMonitorServer_H_
#define _SmartHomeMonitorServer_H_

#include "Threadpool.hpp"         //线程池
#include "TcpServer.hpp"          //TCP服务

class SmartHomeMonitorServer
{
public:
    SmartHomeMonitorServer(int threadNum,int taskSize,unsigned short port,const string & ip = "0.0.0.0");

    //~SmartHomeMonitorServer() {}

    void start();
    void stop();

private:
    void onConnection(wd::TcpConnectionPtr conn);
    void onMessage(wd::TcpConnectionPtr conn);
    void onClose(wd::TcpConnectionPtr conn);

private:
    wd::Threadpool _threadpool;
    wd::TcpServer  _server;
};

#endif

