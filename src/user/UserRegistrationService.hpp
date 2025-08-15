#ifndef USER_REGISTRATION_SERVICE_HPP 
#define USER_REGISTRATION_SERVICE_HPP
#include "MySQLClient.hpp"
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
using std::string;
using std::unordered_map;
using std::mutex;

namespace  wd{

class UserRegistrationService{
public:
     UserRegistrationService();
    /**
     * @brief 处理注册阶段1请求（用户名验证与盐值生成）
     * @param username 输入用户名
     * @param salt 输出生成的盐值
     * @return bool 处理结果（成功/用户名重复）
     */
    bool handleStage1(const string & username, string &salt);
    /**
     * @brief 处理注册阶段2请求（用户数据存储）
     * @param username 输入用户名
     * @param encryptedPassword 加密后的密码
     * @return bool 处理结果（成功/失败）
     */
    bool handleStage2(const string &username,const string &encryptedPassword);
    /**
     * @brief 配置数据库连接参数
     * @param host 数据库主机地址
     * @param port 数据库端口号
     * @param user 数据库用户名
     * @param passwd 数据库密码
     * @param db 数据库名称
     */

private:
    /**
     * @brief 检查用户名可用性
     * @param username 待检查用户名
     * @return bool 用户名是否可用
     */
  bool  isUsernameAvailable(const string & username);
  /**
     * @brief 生成随机盐值
     * @return std::string 8字符随机盐值
     */
  string generateSalt();
   /**
     * @brief 保存用户数据到数据库
     * @param username 用户名
     * @param salt 盐值
     * @param encryptedPassword 加密密码
     * @return bool 存储操作结果
     */
  bool  saveUser(const std::string& username, 
             const std::string& salt,
             const std::string& encryptedPassword);
private:
  // 数据库配置参数
    string _dbHost;
    unsigned short _dbPort;
    string _dbuser;
    string _dbPasswd;
    string _dbdatabase;
     // 临时存储用户名-盐值映射
    unordered_map<string,string> _tempSalt;
    mutex _saltMutex;

};

}//namespace wd 


#endif //__USER_REGISTRATION_SERVICE_HPP

