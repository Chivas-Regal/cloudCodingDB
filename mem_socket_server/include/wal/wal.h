#ifndef _INCLUDE_WAL_WAL_H_
#define _INCLUDE_WAL_WAL_H_

#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include "utils.h"
#include "entry.h"
#include "manifest.h"

struct _manifest_record;

class WAL {
public:
    WAL (const std::string& file_path);
    ~WAL () = default;
    void clear ();
public:
    std::string file_path_;
    std::shared_ptr<std::ofstream> fout_;
    std::shared_ptr<std::ifstream> fin_;
    Slice content_;
};

class WAL_MEMTABLE : public WAL {
public:
    WAL_MEMTABLE (const std::string& file_path) : WAL(file_path) {}
    std::vector<Entry> getElements ();
    void putElement (const std::string& key, const std::string& value);
};


/**
 * 0: record 的添加事件
 * 1：record 的删除事件
 * 2：insert_filei_id 的修改
 */
class WAL_MANIFEST : public WAL {
public:
    WAL_MANIFEST (const std::string& file_path) : WAL(file_path) {}
    std::vector<std::pair<int, std::string>> getElements ();    
    void putElement (int ope, _manifest_record record);
    void putElement (int insert_fil_id);
friend class ManiFest;
};

#endif