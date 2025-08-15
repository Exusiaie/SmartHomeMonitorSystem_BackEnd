#include "BusinessService.hpp"
#include "MyLogger.hpp"

#include <shadow.h>
#include <string.h>
#include <iostream>
using std::cout;
using std::endl;

//该函数运行在计算线程----------------------------------------------------------------------
void UserLoginSection1::process(){
    cout << "login section 1 is processing" << endl;
    
    //判断是否是在，执行用户登录的阶段1，并进行操作
    if(_packet.type == wd::TASK_TYPE_LOGIN_SECTION1){
        string username = _packet.msg;               //消息内容为用户名
        
        //网络版本-------------------------------------------
        //判断用户名是否存在
        //从数据库中取出盐值
        
        //Linux的密码验证功能-------------------------------------------
        struct spwd * sp;                    //从/etc/shadow文件中获取密文?
        sp = getspnam(username.c_str());    //传递一个用户名,就可以得到密文信息，c_str=字符串\0
            if(sp == nullptr){              //当得到的密文为空
               LogDebug("getspnam error");  //调试日志，记录错误位置
               wd::TLV tlv;
                tlv.type = wd::TASK_TYPE_LOGIN_SECTION1_RESP_ERROR; //第一阶段的错误
                tlv.length = 0;                                    //不需要发送消息体了
               _conn->sendInLoop(tlv);//TcpConnection.sendInLoop->Evenloop.runInloop
                return;
            }                                                                                    
    
            //获取要发送给对端的setting(盐值)
            string setting;
           getSetting(setting,sp->sp_pwdp);        //密码在结构体spwd中的内容，放到setting
           LogDebug("setting:%s\n",setting.c_str());
    
            //构造TLV，发送给对端（成功）------------------------------------------
            wd::TLV tlv;
            tlv.type = wd::TASK_TYPE_LOGIN_SECTION1_RESP_OK;
            tlv.length = setting.length();
            strncpy(tlv.data,setting.c_str(),tlv.length);
            _conn->sendInLoop(tlv);//TcpConnection.sendInLoop->EventLoop.runInLoop->TcpConnection.send->SocketIO.writen
        }//if
}              

//获取设置---------------------------
void UserLoginSection1::getSetting(string & s,const char * passwd){
    int i,j;

    //取出salt
    for(i = 0,j = 0;passwd[i]&& j!= 4;++i){    //i记录密码字符下标
        if(passwd[i] == '$'){                  //j记录$出现次数，通常≤3个$符号
            ++j;
        }//if
    }//for

    char buff[128] = {0};
    strncpy(buff,passwd,i);                 //passwd的前i个字符，放到buff中 
    s.assign(buff,i);                        //buff的前i个字符，赋值给字符串s
}

//业务逻辑的处理？-------------------------------------------------------------
void UserLoginSection2::process(){
    cout << "login section 2 is processing" << endl;


    //判断是否是在，执行用户登录的阶段2，并进行操作
    if(_packet.type == wd::TASK_TYPE_LOGIN_SECTION2){
        //保存密文，告知客户端注册成功，返回一个注册成功的消息

        

            //构造TLV，发送给对端（成功）
            wd::TLV tlv;
            tlv.type = wd::TASK_TYPE_LOGIN_SECTION1_RESP_OK;
            tlv.length = setting.length();
            strncpy(tlv.data,setting.c_str(),tlv.length);
            _conn->sendInLoop(tlv);//TcpConnection.sendInLoop->EventLoop.runInLoop->TcpConnection.send->SocketIO.writen

        }//if
}


