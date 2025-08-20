#ifndef _BusinessService_H_
#define _BusinessService_H_ 

#include "Message.hpp"
#include "TcpConnection.hpp"

//用户登录部分1---------------------------------------------------------
class UserLoginSection1
{
public:
    //构造函数                tcp指针                
    UserLoginSection1(wd::TcpConnectionPtr conn,const wd::Packet & p)
    :_conn(conn)
    ,_packet(p)
    {

    }

    //业务逻辑的处理？
    void process();

private:
    //设置
    void getSetting(string & s,const char * passwd);

private:
    wd::TcpConnectionPtr _conn;
    wd::Packet           _packet;
};

//用户登录部分2------------------------------------------------------------
class UserLoginSection2
{
public:
    //构造函数
    UserLoginSection2(wd::TcpConnectionPtr conn,const wd::Packet & p)
    :_conn(conn)
    ,_packet(p)
    {

    }

    //业务逻辑的处理？
    void process();

private:
    wd::TcpConnectionPtr _conn;
    wd::Packet           _packet;
};



class UserRegisterSection1 {
public:
    UserRegisterSection1(wd::TcpConnectionPtr conn, const wd::Packet & p)
    :_conn(conn), _packet(p) {}

    void process();
private:
    wd::TcpConnectionPtr _conn;
    wd::Packet           _packet;
};

class UserRegisterSection2 {
public:
    UserRegisterSection2(wd::TcpConnectionPtr conn, const wd::Packet & p)
    :_conn(conn), _packet(p) {}

    void process();
private:
    wd::TcpConnectionPtr _conn;
    wd::Packet           _packet;
};
#endif

