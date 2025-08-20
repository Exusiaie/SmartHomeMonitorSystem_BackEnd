#include "BusinessService.hpp"
#include "UserLoginService.hpp"
#include "MyLogger.hpp"  // 包含MyLogger头文件
#include "UserRegistrationService.hpp"  // 包含UserRegistrationService头文件
#include <cstring>
using namespace wd;

void UserLoginSection1::process() {
    cout << "login section 1 is processing" << endl;
    
    // 判断是否在执行用户登录阶段1
    if(_packet.type == wd::TASK_TYPE_LOGIN_SECTION1) {
        string username = _packet.msg;
        
        // 直接调用数据库服务获取盐值
        string setting, encryptedPassword;
        bool found = UserloginService::getInstance()
                    ->getUserCredentials(username, setting, encryptedPassword);
        
        if (!found) {
            LogDebug("User not found in DB: %s", username.c_str());
            wd::TLV tlv;
            tlv.type = wd::TASK_TYPE_LOGIN_SECTION1_RESP_ERROR;
            tlv.length = 0;
            _conn->sendInLoop(tlv);
            return;
        }
         // 新增：存储加密密码到连接上下文
        _conn->setContext("login_pass", encryptedPassword);
         // 发送盐值给客户端
        LogDebug("Setting for %s: %s", username.c_str(), setting.c_str());
        wd::TLV tlv;
        tlv.type = wd::TASK_TYPE_LOGIN_SECTION1_RESP_OK;
        tlv.length = setting.length();
        tlv.data.resize(tlv.length);
        strncpy(tlv.data.data(), setting.c_str(), tlv.length);
        _conn->sendInLoop(tlv);
    }
}

void UserLoginSection2::process() {
    cout << "login section 2 is processing" << endl;

    // 直接发送登录成功响应
    if(_packet.type == wd::TASK_TYPE_LOGIN_SECTION2) {
 // 检查上下文是否存在
        if (!_conn->hasContext("login_pass")) {
            LogError("Missing password context");
            wd::TLV tlv;
            tlv.type = wd::TASK_TYPE_LOGIN_SECTION2_RESP_ERROR;
            tlv.length = 0;
            _conn->sendInLoop(tlv);
            return;
        }
        
        // 获取存储的密码
        string storedPass = _conn->getContext("login_pass");
        string clientPass = _packet.msg;
        
        // 安全清零函数（可选）
        auto safeClear = [](string& str) {
            if (!str.empty()) {
                memset(&str[0], 0, str.size());
                str.clear();
            }
        };
        
        // 比较密码
        wd::TLV tlv;
        bool match = (storedPass == clientPass);
        
        // 立即清理敏感数据
        _conn->clearContext("login_pass");
        safeClear(storedPass);
        
        if (match) {
            LogInfo("Login successful");
            tlv.type = wd::TASK_TYPE_LOGIN_SECTION2_RESP_OK;
            tlv.length = 0;
        } else {
            LogWarn("Password mismatch");
            tlv.type = wd::TASK_TYPE_LOGIN_SECTION2_RESP_ERROR;
            tlv.length = 0;
        }
        _conn->sendInLoop(tlv);
        
        // 清理客户端数据
        safeClear(clientPass);
    }

        //wd::TLV tlv;
        //tlv.type = wd::TASK_TYPE_LOGIN_SECTION2_RESP_OK;
        //tlv.length = 0;  // 不需要消息体
       // _conn->sendInLoop(tlv);
    }


void UserRegisterSection1::process() {
    if(_packet.type == wd::TASK_TYPE_REGISTER_SECTION1) {
        string username = _packet.msg;
        
        // 使用单例访问方法
        string salt;
        bool available = wd::UserRegistrationService::getInstance()
                         ->handleStage1(username, salt);
        
        // 构造响应
        wd::TLV tlv;
        if(available) {
            tlv.type = wd::TASK_TYPE_REGISTER_SECTION1_RESP_OK;
            tlv.length = salt.length();
            tlv.data.resize(tlv.length); 
            strncpy(tlv.data.data(), salt.c_str(), tlv.length);
        } else {
            tlv.type = wd::TASK_TYPE_REGISTER_SECTION1_RESP_ERROR;
            tlv.length = 0;
        }
        _conn->sendInLoop(tlv);
    }
}

void UserRegisterSection2::process() {
    if(_packet.type == wd::TASK_TYPE_REGISTER_SECTION2) {
        // 解析用户名和加密密码
        size_t pos = _packet.msg.find('\n');
        if(pos == string::npos) {
            wd::TLV tlv;
            tlv.type = wd::TASK_TYPE_REGISTER_SECTION2_RESP_ERROR;
            tlv.length = 0;
            _conn->sendInLoop(tlv);
            return;
        }
        
        string username = _packet.msg.substr(0, pos);
        string encryptedPass = _packet.msg.substr(pos+1);
        
        // 使用单例访问方法
        bool success = wd::UserRegistrationService::getInstance()
                      ->handleStage2(username, encryptedPass);
        
        // 构造响应
        wd::TLV tlv;
        tlv.type = success ? wd::TASK_TYPE_REGISTER_SECTION2_RESP_OK 
                           : wd::TASK_TYPE_REGISTER_SECTION2_RESP_ERROR;
        tlv.length = 0;
        _conn->sendInLoop(tlv);
    }
}

