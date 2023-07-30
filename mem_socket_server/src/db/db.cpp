#include "db.h"
#include "proj_info.h"
#include <sys/stat.h>

DB::DB(const std::string& username) : 
        username_(username),
        db_impl_(Option::CACHE_SIZE, Option::SST_SIZE, username)
{
    std::string folder_path = std::string(PROJECT_PATH) + "/userdata/" + username;
    if (access(folder_path.c_str(), 0) == -1) {
        mkdir(folder_path.c_str(), S_IRWXU);
    }
}

DB::~DB() {
    close();
}

void DB::put(const std::string& key, std::pair<uint8_t*, size_t> value, bool new_kv) {
    db_impl_.put(key, value, new_kv);
}

void DB::put(const std::string& key, const std::string& value, bool new_kv) {
    db_impl_.put(key, value, new_kv);
}

std::pair<uint8_t*, size_t> DB::get (const std::string& key) {
    return db_impl_.get(key);
}

void DB::erase (const std::string& key) {
    db_impl_.erase(key);
}

void DB::close() {
    db_impl_.close();
}