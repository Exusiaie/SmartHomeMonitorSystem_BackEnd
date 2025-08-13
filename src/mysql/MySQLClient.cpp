#include "MySQLClient.hpp"
#include <iostream>
#include <stdexcept>
using std::string;
using std::cout;
using std::endl;
namespace wd {

/**
 * @brief 构造函数实现
 * 
 * 初始化MySQL连接对象，为后续数据库操作做准备。
 * 如果mysql_init返回NULL，表示初始化失败，抛出runtime_error异常。
 */
MySQLClient::MySQLClient() {
    MYSQL * pconn = mysql_init(&_conn);
    if (!pconn){
        //本处日志在一期日志类完成后更改为日志类
        throw std::runtime_error("MySQL initialization failed: " + 
                                std::string(mysql_error(&_conn)));
    }
}

/**
 * @brief 析构函数实现
 * 
 * 关闭MySQL连接，释放所有相关资源。
 * 确保在任何情况下都能安全释放数据库连接。
 */
MySQLClient::~MySQLClient() {
    mysql_close(&_conn);
}

/**
 * @brief 连接MySQL数据库实现
 * 
 * 使用mysql_real_connect建立实际数据库连接。
 * 连接参数使用C风格字符串传递，确保与MySQL C API兼容。
 * 
 * @return 连接成功返回true，失败返回false
 */
bool MySQLClient::connect(const std::string& host, unsigned short port,
                         const std::string& user, const std::string& passwd,
                         const std::string& db) {
    
    MYSQL * pconn = mysql_real_connect(&_conn, host.c_str(),
                                       user.c_str(), 
                                       passwd.c_str(),
                                       db.c_str(), 
                                       port, 
                                       nullptr,
                                       0);
    if(!pconn)
    {
        return false;
    }
    return true;
}

/**
 * @brief 写操作实现
 * 
 * 使用mysql_real_query执行SQL语句，适用于INSERT/UPDATE/DELETE操作。
 * 返回值为0表示执行成功，非0表示失败。
 * 
 * @return 执行成功返回true，失败返回false
 */
bool MySQLClient::writeOperation(const std::string& sql) {
    int ret = mysql_real_query(&_conn, sql.c_str(), sql.size());
    if(ret != 0){
        //此处改为日志
        printf("(%d, %s)\n", mysql_errno(&_conn), mysql_error(&_conn));
        return false;
    }else{
    //此处改为日志
    printf("Query OK, %ld row(s) affected.\n",mysql_affected_rows(&_conn));
    return true;
    }
}

/**
 * @brief 读操作实现
 * 
 * 执行SQL查询并处理结果集：
 * 1. 执行SQL查询
 * 2. 获取结果集
 * 3. 提取列名作为结果集第一行
 * 4. 逐行读取数据
 * 5. 使用智能指针确保结果集安全释放
 * 
 * @exception std::runtime_error 查询失败或结果处理失败时抛出
 */
std::vector<std::vector<std::string>> MySQLClient::readOperation(const std::string& sql) {
    // 执行SQL查询
    int ret = mysql_real_query(&_conn, sql.c_str(), sql.size());
    if(ret != 0){
        //此处改为日志
        throw std::runtime_error("Query failed: " + std::string(mysql_error(&_conn)));
    }

    // 使用unique_ptr管理结果集资源，确保异常安全
    std::unique_ptr<MYSQL_RES, decltype(&mysql_free_result)> res(
        mysql_store_result(&_conn), mysql_free_result);
    
    // 检查结果集有效性
    if (!res) {
        //此处改为日志
        throw std::runtime_error("Store result failed: " + 
                                std::string(mysql_error(&_conn)));
    }

    std::vector<std::vector<std::string>> results;
    const int num_fields = mysql_num_fields(res.get());
    
    // 获取列名作为结果集第一行
    std::vector<std::string> col_names;
    MYSQL_FIELD* field;
    while ((field = mysql_fetch_field(res.get()))) {
        col_names.push_back(field->name);
    }
    results.push_back(std::move(col_names));

    // 逐行读取数据
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        std::vector<std::string> current_row;
        current_row.reserve(num_fields);
        
        for (int i = 0; i < num_fields; ++i) {
            current_row.push_back(row[i] ? row[i] : "NULL");
        }
        results.push_back(std::move(current_row));
    }
    
    return results;
}

/**
 * @brief 结果集打印实现
 * 
 * 将结果集以表格形式输出到标准输出：
 * - 每行数据用制表符分隔
 * - 第一行为列名
 * - 后续行为实际数据
 */
void MySQLClient::dumpResults(const std::vector<std::vector<std::string>>& res) {
    for (const auto& row : res) {
        for (const auto& col : row) {
            std::cout << col << "\t";
        }
        std::cout << "\n";
    }
}

} // namespace wd
