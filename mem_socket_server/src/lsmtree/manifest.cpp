#include "manifest.h"
#include "option.h"
#include "proj_info.h"
#include <streambuf>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <filesystem>

ManiFest::ManiFest(const std::string& username) :
        _username(username),
        _file_path(std::string(PROJECT_PATH) + "/userdata/" + username + "/manifest.log"),
        wal_manifest(new WAL_MANIFEST(std::string(PROJECT_PATH) + "/userdata/" + username + "/WAL_manifest.log"))
{
    std::ifstream _fin(_file_path);
    _content = std::string((std::istreambuf_iterator<char>(_fin)),
                            std::istreambuf_iterator<char>());
    Slice content_slice = _content;
    while (content_slice.size() > 1) {
        _manifest_record rec(content_slice);
        if (rec._level_id >= _levels_files.size())
            _levels_files.resize(rec._level_id + 1),
            _levels_filenames.resize(rec._level_id + 1);
        _levels_files[rec._level_id].insert(rec);
        _levels_filenames[rec._level_id].insert(rec._file_name);
    }
    insert_file_id = content_slice.empty() ? 1 : content_slice[0];
    _fin.close();
    
    if (wal_manifest->content_.size()) {
        for (auto it : wal_manifest->getElements()) {
            if (it.first == 0) {
                _manifest_record rec = it.second;
                _levels_files[rec._level_id].erase(rec);
                _levels_filenames[rec._level_id].erase(rec._file_name);
            } else if (it.first == 1) {
                _manifest_record rec = it.second;
                _levels_files[rec._level_id].erase(rec);
                _levels_filenames[rec._level_id].erase(rec._file_name);
            } else {
                insert_file_id = getFixed32(it.second);
            }
        }
    }
}

ManiFest::~ManiFest () {
    wal_manifest->clear();
    _levels_files.clear();
}

std::vector<_manifest_record> ManiFest::crossIntersect(const _manifest_record& merger) {
    if (merger._level_id >= _levels_files.size())
        return {};
    std::vector<_manifest_record> ret = {merger};
    if (merger._level_id + 1 >= _levels_files.size())
        return ret;
    std::shared_ptr<__SkipListNode<_manifest_record>> p = _levels_files[merger._level_id + 1].lower(merger);
    if (p->value._file_name.empty())
        return ret;
    while (p) {
        ret.push_back(p->value);
        if (p->value._max_key >= merger._max_key)
            break;
        p = p->right;
    }
    return ret;
}

void ManiFest::flush() {
    wal_manifest->clear();
    _content.clear();
    for (int i = 0; i < _levels_files.size(); i ++) {
        for (_manifest_record it : _levels_files[i].getElements()) {
            setFixed32(&_content, it._level_id);
            setFixed32(&_content, it._file_name.size());
            _content.append(it._file_name);
            setFixed32(&_content, it._min_key.size());
            _content.append(it._min_key);
            setFixed32(&_content, it._max_key.size());
            _content.append(it._max_key);
        }
    }
    _content += char(insert_file_id);
    std::ofstream _fout(_file_path);
    _fout << _content;
    _fout.flush();
    _fout.close();
}

std::string ManiFest::min_absend_fileid(int level_id) {
    if (level_id >= _levels_filenames.size()) {
        _levels_filenames.resize(level_id + 1);
        _levels_files.resize(level_id + 1);
    }
    for (int i = 1;; i ++) {
        if (!_levels_filenames[level_id].count("data" + std::to_string(i) + ".sst")) {
            return "data" + std::to_string(i) + ".sst";
        }
    }
    return "";
}

void ManiFest::put(TableBuilder& table_builder) {
    if (_levels_files.empty())
        _levels_filenames.resize(1),
        _levels_files.resize(1);
    std::string make_fileid = "data" + std::to_string(insert_file_id) + ".sst";
    insert_file_id = insert_file_id == 5 ? 1 : insert_file_id + 1;
    wal_manifest->putElement(insert_file_id);
    if (access((std::string(PROJECT_PATH) + "/userdata/" + _username + "/0").c_str(), 0) == -1) {
        mkdir((std::string(PROJECT_PATH) + "/userdata/" + _username + "/0").c_str(), S_IRWXU);
    }
    table_builder.flush(std::string(PROJECT_PATH) + "/userdata/" + _username + "/0/" + make_fileid);
    _levels_files[0].insert(_manifest_record(0, make_fileid, table_builder.least_entries[0].key(), table_builder.least_entries.back().key()));
    _levels_filenames[0].insert(make_fileid);
    wal_manifest->putElement(0, _manifest_record(0, make_fileid, table_builder.least_entries[0].key(), table_builder.least_entries.back().key()));
    int lev_idx = 0;
    while (true) {
        int limits = lev_idx == 0 ? 4 : std::stoi("1" + std::string(lev_idx, '0'));
        if (_levels_files[lev_idx].size() > limits) {
            int exist_id = lev_idx == 0 ? insert_file_id : rand() % limits;
            while (!_levels_filenames[lev_idx].count("data" + std::to_string(exist_id) + ".sst"))
                exist_id = rand() % limits;
            for (auto it : _levels_files[lev_idx].getElements()) {
                if (it._file_name == "data" + std::to_string(exist_id) + ".sst") {
                    compact(crossIntersect(it));
                    break;
                }
            }
        } else {
            break;
        }
        lev_idx ++;
    }
}
int getFileCountInFolder(const std::string& folderPath) {
    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            count++;
        }
    }
    return count;
}

void ManiFest::compact(const std::vector<_manifest_record>& files) {
    std::string userdata_path = std::string(PROJECT_PATH) + "/userdata/" + _username;
    std::vector<Table> ssts;
    /* [i]=第i个sst文件 {当前块 id，当前块内所有entries, 当前块的 entry_id} */
    struct sst_prog { 
        int block_id; 
        std::vector<Entry> entries; 
        int entry_id; 
    };
    std::vector<sst_prog> sstinfos;
    int min_level = 100, max_level = 0;
    for (int i = 0; i < files.size(); i ++) {
        Table table(userdata_path + "/" + std::to_string(files[i]._level_id) + "/" + files[i]._file_name);
        ssts.push_back(table);
        sstinfos.push_back({0, Block(ssts[i].getBlock(0), 16).decodeEntries(), 0});
        min_level = std::min(files[i]._level_id, min_level);
    }

    bool new_level = false;
    max_level = min_level + 1;
    if (access((userdata_path + "/" + std::to_string(max_level)).c_str(), 0) == -1) {
        mkdir((userdata_path + "/" + std::to_string(max_level)).c_str(), S_IRWXU);
        _levels_filenames.resize(max_level + 1);
        _levels_files.resize(max_level + 1);
        new_level = true;
    }

    std::string last_merge_key = "";
    int data_tempfile_id = 1;
    TableBuilder table_builder(sqrt(Option::SST_SIZE));
    while (true) {
        Entry cur;
        int sstinfos_id = -1;
        for (int i = 0; i < sstinfos.size(); i ++) {
            if (sstinfos[i].block_id == -1)
                continue;
            if (cur.key().empty() || sstinfos[i].entries[sstinfos[i].entry_id] < cur) {
                sstinfos_id = i;
                cur = sstinfos[i].entries[sstinfos[i].entry_id];
            }
        }
        if (sstinfos_id == -1)
            break;
        if (cur.key() != last_merge_key && (cur.value() != DEL_STRING || !new_level))
            table_builder.addEntry(cur.key(), cur.value());
        last_merge_key = cur.key();
        if (sstinfos[sstinfos_id].entry_id + 1 == sstinfos[sstinfos_id].entries.size()) {
            std::string block = ssts[sstinfos_id].getBlock(sstinfos[sstinfos_id].block_id + 1);
            if (block.empty()) 
                sstinfos[sstinfos_id].block_id = -1;
            else 
                sstinfos[sstinfos_id].block_id ++,
                sstinfos[sstinfos_id].entries = Block(block, 16).decodeEntries(),
                sstinfos[sstinfos_id].entry_id = 0;
        } else {
            sstinfos[sstinfos_id].entry_id ++;
        }
        if (table_builder.size() == Option::SST_SIZE) {
            table_builder.finish();
            std::string file_name = this->min_absend_fileid(max_level);
            table_builder.flush(userdata_path + "/" + std::to_string(max_level) + "/" + file_name);
            this->_levels_files[max_level].insert(_manifest_record(max_level, file_name, table_builder.least_entries[0].key(), table_builder.least_entries.back().key()));
            this->_levels_filenames[max_level].insert(file_name);
            wal_manifest->putElement(0, _manifest_record(max_level, file_name, table_builder.least_entries[0].key(), table_builder.least_entries.back().key()));
            table_builder = TableBuilder(sqrt(Option::SST_SIZE));
        }
    }
    if (table_builder.size()) {
        table_builder.finish();
        std::string file_name = this->min_absend_fileid(max_level);
        table_builder.flush(userdata_path + "/" + std::to_string(max_level) + "/" + file_name);
        this->_levels_files[max_level].insert(_manifest_record(max_level, file_name, table_builder.least_entries[0].key(), table_builder.least_entries.back().key()));
        this->_levels_filenames[max_level].insert(file_name);
        wal_manifest->putElement(0, _manifest_record(max_level, file_name, table_builder.least_entries[0].key(), table_builder.least_entries.back().key()));
    }
    for (auto it : files) {
        remove((userdata_path + "/" + std::to_string(it._level_id) + "/" + it._file_name).c_str());
        _levels_files[it._level_id].erase(it);
        _levels_filenames[it._level_id].erase(it._file_name);
        wal_manifest->putElement(1, it);
    }   
}