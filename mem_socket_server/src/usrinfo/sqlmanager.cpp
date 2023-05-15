#include "../../include/usrinfo/sqlmanager.h"

SqlManager::SqlManager () :
    m_conn(mysql_init(nullptr))
{
    mysql_set_character_set(m_conn, "utf8");
}

SqlManager::~SqlManager () {
    if (m_conn) {
        mysql_close(m_conn);
        m_conn = nullptr;
    }
}

void SqlManager::freeResult () {
    if (m_result) {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}

bool SqlManager::query(const std::string& sql) {
    freeResult();
    if (mysql_query(m_conn, sql.c_str())) {
        return false;
    }
    m_result = mysql_store_result(m_conn);
    return true;
}

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

bool SqlManager::selectDB (const std::string& dbName) {
    return query("use " + dbName);
}

std::vector<std::pair<std::string, std::string>> SqlManager::getRes () {
    std::vector<std::pair<std::string, std::string>> ret;
    for (int i = 0; i < 2; i ++) 
        mysql_fetch_field(m_result);
    while (true) {
        MYSQL_ROW m_row = mysql_fetch_row(m_result);
        if (!m_row) break;
        ret.push_back({m_row[0], m_row[1]});
    }
    return ret;
}

std::vector<std::pair<std::string, std::string>> SqlManager::QueryGetRes (const std::string& sql) {
    QueryGetMutex.lock();
    query(sql);
    std::vector<std::pair<std::string, std::string>> ret = getRes();
    QueryGetMutex.unlock();
    return ret;
}