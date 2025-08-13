#include "BusinessService.hpp"
#include "MyLogger.hpp"

#include <shadow.h>
#include <string.h>
#include <iostream>
using std::cout;
using std::endl;

//业务逻辑的处理？
void UserLoginSection1::process(){
    cout << "login section 1 is processing" << endl;
    
    if(_packet.type == wd::TASK_TYPE_LOGIN_SECTION1){
        string username = _packet.msg;

        struct spwd * sp;
        sp = getspnam()
    }
}

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

#endif

