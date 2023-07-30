#ifndef _INCLUDE_UTILS_FILE_H_
#define _INCLUDE_UTILS_FILE_H_

#include <string>
#include <fstream>

class FileReader {
public:
    FileReader (const std::string& file_path);
    ~FileReader ();
    
    std::string read(int offset, size_t size);
private:
    std::ifstream __file_h_fin;
};

class FileWriter {
public:
    FileWriter (const std::string& file_path);
    ~FileWriter ();

    void write (int offset, const std::string& content);
private:
    std::ofstream __file_h_fout;
};

#endif
