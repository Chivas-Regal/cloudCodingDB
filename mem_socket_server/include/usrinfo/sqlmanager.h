#pragma once

#include <mysql.h>
#include <string>
#include <vector>
#include <mutex>

/**
 * @brief mysql API封装
 * 
 * @details 完成对mysql的连接
 *          选择数据库执行sql语句
 *          打印sql语句执行结果
 */
class SqlManager {
public:
    /* 初始化数据库API类 m_conn 但不连接数据库 */
    SqlManager ();

    /* 清除结果集 m_result ，关闭连接 */
    ~SqlManager ();
    
    /* 清除结果集 */
    void freeResult ();

    /* 对 m_conn 执行 sql 语句并将结果储存进 m_result */
    bool query (const std::string& sql);

    /* 连接 mysql(ip地址，端口，用户名，密码) */
    bool connect (
        const std::string& ip,
        unsigned int       port,
        const std::string& user,
        const std::string& passwd);

    /* 选择数据库 */
    bool selectDB (const std::string& dbName);
    
    /* 返回当前结果 */
    std::vector<std::pair<std::string, std::string>> getRes ();

    /* 执行 sql 语句后返回查询结果 */
    std::vector<std::pair<std::string, std::string>> QueryGetRes (const std::string& sql);
private:
    MYSQL*      m_conn;         ///< mysql的api类
    MYSQL_RES*  m_result;       ///< 储存查询结果
    std::mutex QueryGetMutex;   ///< 对查询上锁
};