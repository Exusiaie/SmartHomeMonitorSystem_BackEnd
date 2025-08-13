#include "MySQLClient.hpp"
#include "UserTableCreate.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace wd;
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

// 使用与TestMySQL.cpp相同的数据库配置
constexpr const char* DB_HOST = "192.168.23.128";
constexpr unsigned short DB_PORT = 3306;
constexpr const char* DB_USER = "root";
constexpr const char* DB_PASS = "Wyx@haut0320.";
constexpr const char* DB_NAME = "school";  // 使用原有数据库

// 临时测试表名 - 确保不影响原有数据
constexpr const char* TEST_TABLE_NAME = "test_users_temp";

/**
 * @brief 创建临时测试表
 * 
 * 使用与users表相同的结构创建临时测试表
 */
bool createTempTestTable(MySQLClient& db) {
    const string sql = R"(
        CREATE TABLE IF NOT EXISTS )" + string(TEST_TABLE_NAME) + R"( (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(20) UNIQUE NOT NULL,
            setting CHAR(64) NOT NULL,
            encrypt CHAR(64) NOT NULL
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    return db.writeOperation(sql);
}

/**
 * @brief 删除临时测试表
 * 
 * 测试完成后清理临时表
 */
bool dropTempTestTable(MySQLClient& db) {
    const string sql = "DROP TABLE IF EXISTS " + string(TEST_TABLE_NAME);
    return db.writeOperation(sql);
}

/**
 * @brief 测试用户表创建功能
 */
void testUserTableCreation() {
    cout << "===== 开始用户表创建功能测试 =====" << endl;
    
    MySQLClient dbClient;
    if (!dbClient.connect(DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_NAME)) {
        throw std::runtime_error("数据库连接失败");
    }
    cout << "成功连接到数据库: " << DB_NAME << endl;
    
    // 创建临时测试表
    cout << "\n[步骤1] 创建临时测试表: ";
    if (createTempTestTable(dbClient)) {
        cout << "成功 (" << TEST_TABLE_NAME << ")" << endl;
    } else {
        throw std::runtime_error("创建临时测试表失败");
    }
    
    // 测试表创建功能
    cout << "\n[测试1] 创建用户表: ";
    if (UserTableCreator::createTable(dbClient)) {
        cout << "成功" << endl;
    } else {
        throw std::runtime_error("创建用户表失败");
    }
    
    // 验证表是否存在
    cout << "\n[测试2] 验证表存在: ";
    if (UserTableCreator::tableExists(dbClient)) {
        cout << "成功 - 用户表存在" << endl;
    } else {
        throw std::runtime_error("用户表不存在");
    }
    
    // 测试幂等性（重复创建）
    cout << "\n[测试3] 幂等性测试（重复创建）: ";
    if (UserTableCreator::createTable(dbClient)) {
        cout << "成功 - 重复创建未报错" << endl;
    } else {
        throw std::runtime_error("重复创建失败");
    }
    
    // 测试表删除功能
    cout << "\n[测试4] 删除用户表: ";
    if (UserTableCreator::dropTable(dbClient)) {
        cout << "成功" << endl;
    } else {
        throw std::runtime_error("删除用户表失败");
    }
    
    // 验证表已删除
    cout << "\n[测试5] 验证表删除: ";
    if (!UserTableCreator::tableExists(dbClient)) {
        cout << "成功 - 用户表已删除" << endl;
    } else {
        throw std::runtime_error("用户表仍然存在");
    }
    
    // 清理临时测试表
    cout << "\n[步骤2] 清理临时测试表: ";
    if (dropTempTestTable(dbClient)) {
        cout << "成功 (" << TEST_TABLE_NAME << ")" << endl;
    } else {
        cerr << "警告：清理临时测试表失败" << endl;
    }
    
    cout << "\n===== 所有测试通过! =====" << endl;
}

int main() {
    try {
        testUserTableCreation();
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        cerr << "\n!!! 测试失败: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
