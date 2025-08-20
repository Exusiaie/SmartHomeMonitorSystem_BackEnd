#include "UserLoginService.hpp"
#include "Configuration.hpp"
#include "MyLogger.hpp"
#include <stdexcept>

namespace wd{
UserloginService::UserloginService() {
    // 从配置加载数据库参数
    auto config = Configuration::getInstance();
    auto& configMap = config->getConfigMap();
    _dbHost = configMap["db_host"];
    _dbPort = static_cast<unsigned short>(std::stoi(configMap["db_port"]));
    _dbUser = configMap["db_user"];
    _dbPasswd = configMap["db_passwd"];
    _dbName = configMap["db_database"];

    LogInfo("Database config: host=%s, port=%d, user=%s, db=%s",
            _dbHost.c_str(), _dbPort, _dbUser.c_str(), _dbName.c_str());
}

bool UserloginService::loginStage1(const string & username,string & setting){
    LogInfo("Login Stage1 started for: %s", username.c_str());
    if(username.empty()){
        LogWarn("Username is empty");
        return false;
    }

    string encryptedPassword;
    if(!getUserCredentials(username,setting,encryptedPassword)){
        LogWarn("User credentials not found: %s", username.c_str());
        return false;
    }

    {
        lock_guard<mutex>lock(_saltMutex);
        _tempSalt[username] = setting;
    }

    return true;
}

bool UserloginService::loginStage2(const string & username,const string & encryptedPassword){
    LogInfo("Login Stage2 started for: %s", username.c_str());
    string storedSetting;
    {
        std::lock_guard<mutex> lock(_saltMutex);
        auto it = _tempSalt.find(username);
        if(it == _tempSalt.end()){
            LogError("Temporary salt not found for: %s", username.c_str());
            return false;
        }
        storedSetting = it->second;
        _tempSalt.erase(username);
    }
    string currentSetting,storedPassword;
    if(!getUserCredentials(username,currentSetting,storedPassword)){
        LogWarn("Current credentials not found: %s", username.c_str());
        return false;
    }
    if(storedSetting != currentSetting){
        LogWarn("Setting mismatch for: %s (stored: %s, current: %s)", 
                username.c_str(), storedSetting.c_str(), currentSetting.c_str());
        return false;
    }
    if(storedPassword == encryptedPassword){
        LogInfo("Login successful for: %s", username.c_str());
        return true;
    }else{
        LogWarn("Password mismatch for: %s", username.c_str());
        return false;
    }
}


bool UserloginService::getUserCredentials(const std::string& username,
                                    std::string& setting,
                                    std::string& encryptedPassword) {
    MySQLClient db;
    if(!db.connect(_dbHost, _dbPort, _dbUser, _dbPasswd, _dbName)){
        LogError("Database connection failed");
        return false;
    }

    string sql = "SELECT setting, encrypt FROM users WHERE name = '" + username + "'";

    try {
        auto result = db.readOperation(sql);

        // 新的结果处理逻辑
        // 结果集应至少包含列名行(1行) + 数据行(至少1行)
        if (result.size() < 2) {
            LogWarn("User not found: %s, result size: %zu",
                    username.c_str(), result.size());
            return false;
        }

        // 查找数据行（跳过列名行）
        for (size_t i = 1; i < result.size(); ++i) {
            const auto& row = result[i];
            if (row.size() >= 2) {
                setting = row[0];
                encryptedPassword = row[1];
                return true;
            }
        }

        LogWarn("User found but missing data fields: %s", username.c_str());
        return false;

    } catch(const std::exception& e) {
        LogError("User credential query exception: %s", e.what());
        return false;
    }
}

UserloginService* UserloginService::getInstance() {
    static UserloginService instance; // 单例实现
    return &instance;
}
}
