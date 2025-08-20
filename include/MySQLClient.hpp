#ifndef __MySQLClient_H__
#define __MySQLClient_H__
/**
 * @file MySQLClient.hpp
 * @brief MySQL数据库操作封装类
 * 
 * 该类封装了MySQL数据库的基本操作，提供连接管理、SQL查询和结果处理功能。
 * 设计遵循RAII原则，确保资源安全释放。
 * 
 * 主要功能包括：
 * 1. 数据库连接管理
 * 2. SQL写操作（INSERT/UPDATE/DELETE）
 * 3. SQL读操作（SELECT）
 * 4. 查询结果集导出
 * @note 该类禁止拷贝和赋值操作，保证资源唯一性
 */
#include <mysql/mysql.h>
#include <memory>
#include <string>
#include <vector>
using std::string;
using std::vector;

namespace wd
{
class MySQLClient
{
public:
    /**
     * @brief 默认构造函数
     * @exception std::runtime_error 如果初始化失败则抛出异常
     * 
     * 初始化MySQL连接对象，为后续操作做准备。
     * 如果mysql_init失败，会抛出runtime_error异常。
     */
    MySQLClient();
   /**
     * @brief 析构函数
     * 
     * 自动关闭MySQL连接，释放所有相关资源。
     * 遵循RAII原则，确保资源安全释放。
     */
    ~MySQLClient();
   /**
     * @brief 连接MySQL数据库
     * @param host 数据库主机地址
     * @param port 数据库端口号
     * @param user 用户名
     * @param passwd 密码
     * @param db 数据库名称
     * @return bool 连接成功返回true，失败返回false
     * 
     * 使用提供的凭证连接到MySQL服务器，连接失败时返回false。
     * 连接成功后，后续操作将在该连接上进行。
     */
    bool connect(const string & host,
                 unsigned short port,
                 const string & user,
                 const string & passwd,
                 const string & db);
    /**
     * @brief 执行写操作（INSERT/UPDATE/DELETE）
     * @param sql SQL语句字符串
     * @return bool 执行成功返回true，失败返回false
     * 
     * 执行不返回结果集的SQL语句，适用于数据修改操作。
     * 失败时会返回false，调用者可通过mysql_error获取详细信息。
     */
    bool writeOperation(const string & sql);
     /**
     * @brief 执行读操作（SELECT）
     * @param sql SQL查询语句
     * @return std::vector<std::vector<std::string>> 查询结果集
     * @exception std::runtime_error 查询失败抛出异常
     * 
     * 执行SQL查询语句并返回结果集，结果集格式为二维字符串数组：
     * - 第一行包含列名
     * - 后续行包含查询结果数据
     * 
     * 如果查询失败，抛出runtime_error异常，包含MySQL错误信息。
     */
    vector<vector<string>> readOperation(const string & sql);
 /**
     * @brief 静态方法：打印查询结果集
     * @param res 要打印的结果集
     * 
     * 将查询结果以表格形式输出到标准输出流，便于调试和查看：
     * - 第一行显示列名
     * - 后续行显示数据值
     * - 列之间用制表符分隔
     */
    static void dumpResults(const vector<vector<string>> & res);
    // 禁用拷贝和赋值
    MySQLClient(const MySQLClient&) = delete;      ///< 禁用拷贝构造函数
    MySQLClient& operator=(const MySQLClient&) = delete;  ///< 禁用赋值运算符
private:
     /**
     * @brief 初始化数据库连接（内部使用）
     * 
     * 在构造函数中调用，完成MySQL连接对象的初始化工作。
     * 如果初始化失败，会抛出异常。
     */
    void init();

private:
    MYSQL _conn;
};

}//end of namesapce wd
#endif
