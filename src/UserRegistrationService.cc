#include "UserRegistrationService.hpp"
#include "UserTableCreate.hpp"
#include "Configuration.hpp"
#include "MyLogger.hpp"
#include <random>
#include <ctime>
#include <stdexcept>//临时

namespace wd{

UserRegistrationService::UserRegistrationService() {
    // 从配置加载数据库参数
    auto config = Configuration::getInstance();
    auto& configMap = config->getConfigMap();
    _dbHost = configMap["db_host"];
    _dbPort = static_cast<unsigned short>(std::stoi(configMap["db_port"]));
    _dbuser = configMap["db_user"];
    _dbPasswd = configMap["db_passwd"];
    _dbdatabase = configMap["db_database"];

    LogInfo("Database config: host=%s, port=%d, user=%s, db=%s",
            _dbHost.c_str(), _dbPort, _dbuser.c_str(), _dbdatabase.c_str());
}

bool UserRegistrationService::handleStage1(const string & username,string & salt){
    //参数检查
    LogInfo("Registration Stage1 started for: %s", username.c_str());
    if(username.empty()){
        LogWarn("Username is empty");
        return false;
    }
    //
    //用户名查重
    if(!isUsernameAvailable(username)){
        LogWarn("Username already exists: %s", username.c_str());
        return false;
    }
    //生成盐值
    salt = generateSalt();
    LogInfo("Generated salt for %s: %s", username.c_str(), salt.c_str());
    {
    //存储临时盐值
    std::lock_guard<mutex> lock(_saltMutex);
    _tempSalt[username] = salt;
    }
    return true;

}

bool UserRegistrationService::handleStage2(const string &username,const string & encryptedPassword){
   LogInfo("Registration Stage2 started for: %s", username.c_str());
    string salt;
    {
        std::lock_guard<mutex> lock(_saltMutex);
        auto it = _tempSalt.find(username);
        if(it == _tempSalt.end()){
            LogError("Salt not found for user: %s", username.c_str());
            return false;
        }
        salt = it->second;
    }

    bool saveResult = saveUser(username,salt,encryptedPassword);
     if(saveResult) {
        LogInfo("User registered successfully: %s", username.c_str());
    } else {
        LogError("Failed to register user: %s", username.c_str());
    }
    {
    std::lock_guard<mutex> lock(_saltMutex);
    _tempSalt.erase(username);
    }

    return saveResult;
}


bool UserRegistrationService::isUsernameAvailable(const string & username){
   MySQLClient db;
   if(!db.connect(_dbHost,_dbPort,_dbuser,_dbPasswd,_dbdatabase)){
       LogError("Database connection failed");
       throw std::runtime_error("数据库连接失败");
   } 
   //执行用户查询
   std::string sql = "SELECT id FROM users WHERE name = '" + username + "'";
   try{
       auto result = db.readOperation(sql);
       return result.size() <= 1;
   }catch(const std::exception& e){
       LogError("User query exception: %s", e.what());
       throw std::runtime_error("用户查询异常");
   }

}

string UserRegistrationService::generateSalt(){
    const std::string charSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    const int length = 8;
    
    // 安全随机数生成
    std::string salt;
    salt.reserve(length);
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, charSet.size() - 1);
    
    // 生成盐值字符串
    for (int i = 0; i < length; i++) {
        salt += charSet[dist(generator)];
    }
    
    return salt;
}

bool  UserRegistrationService::saveUser(const std::string& username, 
                                     const std::string& salt,
                                     const std::string& encryptedPassword) {
    MySQLClient db;
    if(!db.connect(_dbHost,_dbPort,_dbuser,_dbPasswd,_dbdatabase)){
        LogError("Database connection failed for saving user");
        return false;
    }
    //此处把保存的setting直接是salt(不增加客户端解码难度了)
    string setting =  salt;
    std::string sql = "INSERT INTO users (name, setting, encrypt) VALUES ('"
                    + username + "', '" 
                    + setting + "', '" 
                    + encryptedPassword + "')";
    bool result =  db.writeOperation(sql);
    if(result) {
        LogInfo("User saved to database: %s", username.c_str());
    }
    return result;
    }
}
