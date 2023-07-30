#ifndef _INCLUDE_LSMTREE_MANIFEST_H_
#define _INCLUDE_LSMTREE_MANIFEST_H_

#include <string>
#include <memory>
#include "skiplist.h"
#include "utils.h"
#include "table.h"
#include "wal.h"

class WAL;
class WAL_MEMTABLE;
class WAL_MANIFEST;

/**
 * @brief sst 文件信息记录
 * 
 *   +----------------------+
 *   |      level id     4B |
 *   +----------------------+
 *   |     filename len  4B |
 *   +----------------------+
 *   |       filename       |
 *   +----------------------+
 *   |      minkey len   4B |
 *   +----------------------+
 *   |        minkey        |
 *   +----------------------+
 *   |      maxkey len   4B |
 *   +----------------------+
 *   |        maxkey        |
 *   +----------------------+
 * 
 * 每一条记录代表一张 sst 表的信息：level层数、文件名、最小的key、最大的key
 */
struct _manifest_record {
    int _level_id;          ///< level层数
    std::string _file_name; ///< 文件名
    std::string _min_key;   ///< 最小的key
    std::string _max_key;   ///< 最大的key

    _manifest_record () : _level_id(0), _file_name(""), _min_key(""), _max_key("") {}

    /**
     * @brief 利用 content 构造，直接吃掉 content 的前一条记录
     * 
     * @param content 前缀为一条 sst 文件信息的记录
     */
    _manifest_record (Slice& content) {
        _level_id = getFixed32(content);

        int file_name_len = getFixed32(content);
        _file_name = content.data().substr(0, file_name_len);
        content.remove_prefix(file_name_len);

        int minkey_len = getFixed32(content);
        _min_key = content.data().substr(0, minkey_len);
        content.remove_prefix(minkey_len);

        int maxkey_len = getFixed32(content);
        _max_key = content.data().substr(0, maxkey_len);
        content.remove_prefix(maxkey_len);
    }
    /**
     * @brief 利用已有字符串构造，转变成 slice 再执行上面操作
     * 
     * @param s 一条 sst 文件信息记录
     */
    _manifest_record (const std::string &s) {
        Slice content = s;
        _level_id = getFixed32(content);

        int file_name_len = getFixed32(content);
        _file_name = content.data().substr(0, file_name_len);
        content.remove_prefix(file_name_len);

        int minkey_len = getFixed32(content);
        _min_key = content.data().substr(0, minkey_len);
        content.remove_prefix(minkey_len);

        int maxkey_len = getFixed32(content);
        _max_key = content.data().substr(0, maxkey_len);
        content.remove_prefix(maxkey_len);
    }
    _manifest_record (int level_id, const std::string& file_name, const std::string& min_key, const std::string& max_key) :
        _level_id(level_id),
        _file_name(file_name),
        _min_key(min_key),
        _max_key(max_key) 
    {}

    bool operator < (const _manifest_record& that) const {
        if (_min_key != that._min_key)
            return _min_key < that._min_key;
        else if (_max_key != that._max_key)
            return _max_key < that._max_key;
        else if (_level_id != that._level_id)
            return _level_id < that._level_id;
        return _file_name < that._file_name;
    }
    bool operator == (const _manifest_record& that) const {
        return _min_key   ==  that._min_key  &&
               _max_key   ==  that._max_key  &&
               _level_id  ==  that._level_id &&
               _file_name ==  that._file_name;
    }
    bool operator <= (const _manifest_record& that) const {
        return *this < that || *this == that;
    }
    std::string data () const {
        return _min_key;
    }
    std::string toString () const {
        std::string content;
        setFixed32(&content, _level_id);
        setFixed32(&content, _file_name.size());
        content += _file_name;
        setFixed32(&content, _min_key.size());
        content += _min_key;
        setFixed32(&content, _max_key.size());
        content += _max_key;
        return content;
    }
};

class ManiFest {
public:
    /* 通过 file_path 读取所有内容并分 level 解析元素 */
    ManiFest (const std::string& username);

    /* 析构，主要是将 _levels_files 清了 */
    ~ManiFest ();

    /* 求出在 merger 更深一层的记录中，key 有交集的所有记录 */
    std::vector<_manifest_record> crossIntersect (const _manifest_record& merger);

    /* 将 _levels_files 通过 _content 刷到 ${_file_path} 中 */
    void flush ();

    /* 获取最小未出现的文件名 {data1.sst,data3.sst,...}=data2.sst */
    std::string min_absend_fileid (int level_id);

    void put (TableBuilder& table_builder);
private:
    void compact (const std::vector<_manifest_record>& files);
public:
    std::string _username;  ///< 用户名
    std::string _file_path; ///< manifest 文件路径
    std::vector<SkipList<_manifest_record>> _levels_files; ///< 所有层的 sst 记录们
    std::vector<SkipList<std::string>> _levels_filenames;  ///< 所有层的 sst 文件名们
    std::string _content;   ///< 读取和写入的中转字符串
    int insert_file_id;     ///< 待插入文件 id (1->5 轮转)
    std::shared_ptr<WAL_MANIFEST> wal_manifest; ///< 保障 manifest 的 wal
friend class Compactor;
friend class DBImpl;
};

#endif