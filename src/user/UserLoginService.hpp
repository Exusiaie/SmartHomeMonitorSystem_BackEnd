#ifndef USER_LOGIN_SERVICE_HPP
#define USER_LOGIN_SERVICE_HPP

#include "MySQLClient.hpp"
#include <string>
#include <mutex>
#include <unordered_map>

using std::string;
using std::mutex;
using std::unordered_map;

namespace wd{
class Userloginservice{

public:
    Userloginservice();
    /**
     * @brief 处理登录阶段1请求（获取用户setting）
     * @param username 输入用户名
     * @param setting 输出完整setting字符串(格式: $1$salt$)
     * @return bool 处理结果（成功/用户不存在）
     */
    bool loginStage1(const string & username,string &setting);
     /**
     * @brief 处理登录阶段2请求（密码验证）
     * @param username 输入用户名
     * @param encryptedPassword 加密后的密码（客户端使用完整setting加密）
     * @return bool 验证结果（成功/失败）
     */
    bool loginStage2(const string &username,const string & encryptedPassword);

private:

     bool getUserCredentials(const std::string& username, 
                           std::string& setting, 
                           std::string& encryptedPassword);
private:
      // 数据库配置
    std::string _dbHost;
    unsigned short _dbPort;
    std::string _dbUser;
    std::string _dbPasswd;
    std::string _dbName;

    // 临时存储用户名-setting映射
    unordered_map<string,string>_tempSalt;//加工后的salt
    mutex _saltMutex;


};
}


#endif

