#ifndef _Message_H_ 
#define _Message_H_
#include <vector>
#include <string>

using std::string;

namespace wd{

//枚举类型，任务类型，可以做类型检查-----
enum TaskType{
    TASK_TYPE_LOGIN_SECTION1 = 1,
    TASK_TYPE_LOGIN_SECTION1_RESP_OK,
    TASK_TYPE_LOGIN_SECTION1_RESP_ERROR,
    TASK_TYPE_LOGIN_SECTION2,
    TASK_TYPE_LOGIN_SECTION2_RESP_OK,
    TASK_TYPE_LOGIN_SECTION2_RESP_ERROR,//6
    
    //注册类
    TASK_TYPE_REGISTER_SECTION1 = 10,
    TASK_TYPE_REGISTER_SECTION1_RESP_OK,
    TASK_TYPE_REGISTER_SECTION1_RESP_ERROR,
    TASK_TYPE_REGISTER_SECTION2,
    TASK_TYPE_REGISTER_SECTION2_RESP_OK,
    TASK_TYPE_REGISTER_SECTION2_RESP_ERROR//15
};


//发送时-------------------------------
struct TLV{
    int type;
    int length;
    std::vector<char> data;
};

//解析时-----------------
struct Packet{
    int type;
    int length;
    string msg;
};

}//namespace wd
#endif

