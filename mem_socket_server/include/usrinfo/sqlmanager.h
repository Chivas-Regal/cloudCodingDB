#pragma once

#include <mysql.h>
#include <string>
#include <vector>
#include <mutex>

class SqlManager {
public:
    SqlManager ();
    ~SqlManager ();
    
    void freeResult ();
    bool query (const std::string& sql);
    bool connect (
        const std::string& ip,
        unsigned int       port,
        const std::string& user,
        const std::string& passwd);
    bool selectDB (const std::string& dbName);
    
    std::vector<std::pair<std::string, std::string>> getRes ();
    std::vector<std::pair<std::string, std::string>> QueryGetRes (const std::string& sql);
private:
    MYSQL*      m_conn;
    MYSQL_RES*  m_result;
    std::mutex QueryGetMutex;
};