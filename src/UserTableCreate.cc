#include "UserTableCreate.hpp"
#include "MyLogger.hpp"
#include <string>
#include <stdexcept>//临时
 using std::string;

 namespace wd{

 bool UserTableCreator::createTable(MySQLClient& db) {
     LogInfo("Creating user table...");
     const string sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(20) UNIQUE NOT NULL,
            setting CHAR(64) NOT NULL,
            encrypt CHAR(64) NOT NULL
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
     bool result =  db.writeOperation(sql);
     if(result) {
        LogInfo("User table created successfully");
    } else {
        LogError("Failed to create user table");
    }
    return result;
     }
bool UserTableCreator::dropTable(MySQLClient& db) {
    LogInfo("Dropping user table...");
    const std::string sql = "DROP TABLE IF EXISTS users";
    bool result = db.writeOperation(sql);
     if(result) {
        LogInfo("User table dropped successfully");
    } else {
        LogError("Failed to drop user table");
    }
    return result;
}

bool UserTableCreator::tableExists(MySQLClient& db) {
    try {
        const std::string sql = R"(
            SELECT TABLE_NAME 
            FROM INFORMATION_SCHEMA.TABLES 
            WHERE TABLE_SCHEMA = DATABASE()
            AND TABLE_NAME = 'users'
        )";
        
        auto result = db.readOperation(sql);
        return result.size() > 0; // 存在数据行
    } catch (const std::exception& e) {
        LogError("Table existence check failed: %s", e.what());
        return false;
    }
}

string UserTableCreator::getTableSchema() {
    return R"(
        CREATE TABLE users (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(20) UNIQUE NOT NULL COMMENT '用户名',
            setting CHAR(64) NOT NULL COMMENT '加密设置(前缀+盐值)',
            encrypt CHAR(64) NOT NULL COMMENT 'MD5加密密文'
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
    )";
}

} // namespace wd
