#include "UserTableCreate.hpp"
#include <string>
#include <stdexcept>//临时
 using std::string;

 namespace wd{

 bool UserTableCreator::createTable(MySQLClient& db) {
    const string sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(20) UNIQUE NOT NULL,
            setting CHAR(64) NOT NULL,
            encrypt CHAR(64) NOT NULL
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
     return db.writeOperation(sql);
     }
bool UserTableCreator::dropTable(MySQLClient& db) {
   const std::string sql = "DROP TABLE IF EXISTS users";
    return db.writeOperation(sql);
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
        return result.size() > 1; // 存在数据行
    } catch (const std::exception& e) {
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
