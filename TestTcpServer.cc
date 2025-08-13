#include "TcpConnection.hpp"   //TCP连接
#include "TcpServer.hpp"       //？
#include "Threadpool.hpp""     //线程池
#include <iostream>

using std::cout;
using std::endl;

//创建了，线程池对象     ？
wd::Threadpool threadpool(4,10);

//任务类---------------------------------------------------------------------------
class Mytask{
public:
    //构造函数--------------------------------------
    Mytask(const string & msg,wd::TcpConnectionPtr conn)
    :_msg(msg)
    ,_conn(conn)
    {

    }

    //过程------------------------------------------？
    void process(){
        cout << "Mytask::process is running" << endl;
        string response = _msg;
        _conn->sendInLoop(response);  //TCP.sendInLoop->EventLoop.runInLoop->vector存入数据
    }

private:
    string _msg;                  //消息
    wd::TcpConnectionPtr _conn;   //TCP指针
};

//重复服务--------------------------------------------------------------------------
class EchoServer{
public:
    //构造函数--------------------
    EchoServer(unsigned short port,const string ip,int threadNum,int queSize)
    :_threadpool(threadNum,queSize)
    ,_server(port,ip)
    {
        printf("EchoServer()\n");
        using namespace std::placeholders;   //占位符

        //设置回调函数？
        _server.setAllCallbacks(std::bind(&EchoServer::onConnection,this,_1),
                                std::bind(&EchoServer::onMessage,this,_1),
                                std::bind(&EchoServer::onClose,this,_1));
    }

    //开始------------------------
    void start(){
        _threadpool.start();
        _server.start();
    }

private:
    //-------------------------------------------------------------------------
    void onConnection(wd::TcpConnectionPtr conn){
        cout << conn->toString() << "has connected successfully.\n";
    }

    //执行在IO线程中的，时间不宜过长，否则影响并发处理，不能保证实时性--------
    void onMessage(wd::TcpConnectionPtr conn){
        string msg = conn->receive();          //不能阻塞？
        cout << "recv:" << msg <<endl;

        //decode
        //compute
        //encode
        
        //复杂的业务逻辑，可以交给，计算线程来执行
        Mytask task(msg,conn);
        _threadpool.addTask(std::bind(&Mytask::process,task));

        //当计算线程处理完毕之后，再交给IO线程，进行发送-？
    }

    //---------------------------------------------------------------------
    void onClose(wd::TcpConnectionPtr conn){
        cout << conn->toString() << "has closed." << endl;
    }

private:
    wd::Threadpool _threadpool;     //线程池
    wd::TcpServer  _server;         //TCP服务
};

int main()
{
    EchoServer server(8000,"0.0.0.0",4,10);
    server.start();

    return 0;
}

