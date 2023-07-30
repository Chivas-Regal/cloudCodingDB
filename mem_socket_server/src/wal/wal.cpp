#include "wal.h"
#include <filesystem>

WAL::WAL(const std::string& file_path) :
        file_path_(file_path),
        fout_(std::make_shared<std::ofstream>(file_path, std::ios::app)),
        fin_(std::make_shared<std::ifstream>(file_path)){
    std::stringstream buffer;
    buffer << fin_->rdbuf();
    content_ = buffer.str();
}

void WAL::clear() {
    std::ofstream file(file_path_, std::ios::out | std::ios::trunc);
    file.close();
}



std::vector<Entry> WAL_MEMTABLE::getElements() {
    std::vector<Entry> ret;
    while (content_.size()) {
        int32_t key_size = getFixed32(content_);
        std::string key = content_.data().substr(0, key_size);
        content_.remove_prefix(key_size);
        int32_t value_size = getFixed32(content_);
        std::string value = content_.data().substr(0, value_size);
        content_.remove_prefix(value_size);
        ret.push_back({key, value});
    }
    return ret;
}

void WAL_MEMTABLE::putElement(const std::string& key, const std::string& value) {
    std::string row;
    setFixed32(&row, key.size());
    row.append(key);
    setFixed32(&row, value.size());
    row.append(value);
    fout_->write(row.c_str(), row.size());
    fout_->flush();
}



std::vector<std::pair<int, std::string>> WAL_MANIFEST::getElements() {
    std::vector<std::pair<int, std::string>> ret;
    while (content_.size()) {
        int op = content_[0];
        content_.remove_prefix(1);
        int32_t size = getFixed32(content_);
        std::string str = content_.data().substr(0, size);
        content_.remove_prefix(size);
        ret.push_back({op, str});
    }
    return ret;
}

void WAL_MANIFEST::putElement (int ope, _manifest_record record) {
    std::string row;
    row += char(ope);
    std::string record_str = record.toString();
    setFixed32(&row, record_str.size());
    row += record_str;
    fout_->write(row.c_str(), row.size());
    fout_->flush();
}

void WAL_MANIFEST::putElement(int insert_file_id) {
    std::string row;
    row += char(2);
    setFixed32(&row, insert_file_id);
    fout_->write(row.c_str(), row.size());
    fout_->flush();
}