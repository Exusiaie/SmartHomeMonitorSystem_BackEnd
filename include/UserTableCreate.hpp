#ifndef  USER_TABLE_CREATOR_HPP 
#define  USER_TABLE_CREATOR_HPP 

#include "MySQLClient.hpp"
#include <string>

namespace wd{

class UserTableCreator{
public:
    /**
     * @brief 创建用户表
     * @param db 已连接的MySQLClient实例
     * @return bool 是否创建成功
     *
     * 执行SQL创建用户表：
     */
    static bool createTable(MySQLClient& db);

    /**
     * @brief 删除用户表
     * @param db 已连接的MySQLClient实例
     * @return bool 是否删除成功
     *
     * 警告：此操作会永久删除所有用户数据
     *   DROP TABLE IF EXISTS users;
     */
    static bool dropTable(MySQLClient& db);

    /**
     * @brief 检查用户表是否存在
     * @param db 已连接的MySQLClient实例
     * @return bool 表是否存在
     */
    static bool tableExists(MySQLClient& db);

    /**
     * @brief 获取用户表结构定义
     * @return std::string SQL创建语句
     */
    static std::string getTableSchema();

private:
    //禁用实例化
    UserTableCreator() = delete;
    ~UserTableCreator() = delete;

};
}//end of namespace wd

#endif //USER_TABLE_CREATOR_HPP

