#include "file.h"

FileReader::FileReader(const std::string& file_path) :
    __file_h_fin(file_path) 
{}

FileReader::~FileReader() {
    __file_h_fin.close();
}

std::string FileReader::read (int offset, size_t size) {
    __file_h_fin.seekg(offset);
    char ret[size];
    __file_h_fin.read(ret, size);
    return ret;
}

FileWriter::FileWriter(const std::string& file_path) :
    __file_h_fout(file_path)
{}

FileWriter::~FileWriter() {
    __file_h_fout.close();
}

void FileWriter::write (int offset, const std::string& content) {
    __file_h_fout.seekp(offset);
    __file_h_fout.write(content.data(), content.size());
}