#include "../../include/usrinfo/sqlmanager.h"

/**
 * @brief 构造函数
 * 
 * @details 初始化数据库API类 m_conn 但不连接数据库
 */
SqlManager::SqlManager () :
    m_conn(mysql_init(nullptr))
{
    mysql_set_character_set(m_conn, "utf8");
}

/**
 * @brief 析构函数
 * 
 * @details 清除结果集 m_result ，关闭连接
 */
SqlManager::~SqlManager () {
    freeResult();
    if (m_conn) {
        mysql_close(m_conn);
        m_conn = nullptr;
    }
}

/**
 * @brief 清除结果集
 */
void SqlManager::freeResult () {
    if (m_result) {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}

/**
 * @brief 对 m_conn 执行 sql 语句并将结果储存进 m_result
 * 
 * @param sql 待执行语句
 * @return true  执行成功
 * @return false 执行失败
 */
bool SqlManager::query(const std::string& sql) {
    freeResult();
    if (mysql_query(m_conn, sql.c_str())) {
        return false;
    }
    m_result = mysql_store_result(m_conn);
    return true;
}

/**
 * @brief 连接 mysql
 * 
 * @param ip        mysql连接 ip 地址
 * @param port      mysql连接端口
 * @param user      mysql连接用户名
 * @param passwd    mysql连接密码
 * 
 * @return true     连接成功
 * @return false    连接失败
 */
bool SqlManager::connect(
        const std::string& ip,
        unsigned int       port,
        const std::string& user,
        const std::string& passwd
    )
{
    MYSQL* p = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), "", port, nullptr, 0);
    return p != nullptr;
}

/**
 * @brief 选择数据库
 * 
 * @param dbName 数据库名
 * 
 * @return true     选择成功
 * @return false    选择失败
 * 
 * @details 在 m_conn 中执行 use ... 语句
 */
bool SqlManager::selectDB (const std::string& dbName) {
    return query("use " + dbName);
}

/**
 * @brief 返回当前结果
 * 
 * @return std::vector<std::pair<std::string, std::string>> 返回 infos.mem_socket_login_infos 表中的 {username, password} 信息对
 */
std::vector<std::pair<std::string, std::string>> SqlManager::getRes () {
    std::vector<std::pair<std::string, std::string>> ret;
    for (int i = 0; i < 2; i ++) 
        mysql_fetch_field(m_result);
    /* 一行一行出 */
    while (true) {
        MYSQL_ROW m_row = mysql_fetch_row(m_result);
        if (!m_row) break;
        ret.push_back({m_row[0], m_row[1]});
    }
    return ret;
}

/**
 * @brief 执行 sql 语句后返回查询结果
 * 
 * @param sql 待执行语句
 * @return std::vector<std::pair<std::string, std::string>> 返回 infos.mem_socket_login_infos 表中的 {username, password} 信息对
 */
std::vector<std::pair<std::string, std::string>> SqlManager::QueryGetRes (const std::string& sql) {
    QueryGetMutex.lock();
    query(sql);
    std::vector<std::pair<std::string, std::string>> ret = getRes();
    QueryGetMutex.unlock();
    return ret;
}