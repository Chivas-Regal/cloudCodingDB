#ifndef _INCLUDE_DB_DB_H_
#define _INCLUDE_DB_DB_H_

#include "db_impl.h"
#include "option.h"

class DB {
public:
    DB (const std::string& username);
    ~DB ();

    /* 填入键值对，对于用户层使用参数 key,value 即可，new_kv 表示是否为用户手动添加的，底层调用为false */
    void put (const std::string& key, std::pair<uint8_t*, size_t> value, bool new_kv = true);

    /* 填入键值对，对于用户层使用参数 key,value 即可，new_kv 表示是否为用户手动添加的，底层调用为false */
    void put (const std::string& key, const std::string& value, bool new_kv = true);

    /* 利用 key 访问 value */
    std::pair<uint8_t*, size_t> get (const std::string& key);

    /* 删除键 key */
    void erase (const std::string& key);

    void close ();
private:
    DBImpl db_impl_; 
    std::string username_;   
};

#endif