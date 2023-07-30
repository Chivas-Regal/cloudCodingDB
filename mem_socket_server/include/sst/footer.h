#ifndef _INCLUDE_SST_FOOTER_H_
#define _INCLUDE_SST_FOOTER_H_

#include <string>

/**
 * @details
 *      
 *      +----------------------------+
 * TODO | 8B  metaindex block offset |
 * TODO +----------------------------+
 * TODO | 8B    metaindex block size |
 * TODO +----------------------------+
 *      | 8B      index block offset |
 *      +----------------------------+
 *      | 8B        index block size |
 *      +----------------------------+
 *      | 8B            magic number |
 *      +----------------------------+
 */

/**
 * @brief 尾部编码器
 */
class FooterBuilder {
public:
    /* 初始化一个未添加任何东西的空编码器 */
    FooterBuilder () = default;

    /* 用已有编码串生成（半成品 or 成品）编码器 */
    FooterBuilder (const std::string& content);
    ~FooterBuilder () = default;

    /* 加入 indexblock 的信息：偏移量、大小 */
    void addIndexBlock (uint64_t offset, uint64_t size);

    /* 封底：添加 magic_number */
    void finish (uint64_t magic_number);

    /* 将编码后的 footer 刷到字符串 dst 上（调用前应确保调用了 finish） */
    void flush (std::string* dst);
private:
    /* 编成的字节码 */
    std::string content_;

friend class TableBuilder;
};

/**
 * @brief 尾部解析器
 */
class Footer {
public:
    Footer () = default;
    /* 解析已有字符串 */
    Footer (const std::string& content);
    Footer (const char* content);
    ~Footer() = default;

    /* 当前 offset 仅能 0 ，用 footer 通过 footer 内的偏移量 offset 获取 Table 内 indexBlock 的绝对偏移量和大小 */
    std::pair<uint64_t, uint64_t> getBlock (uint64_t offset);

    /* 获取 magic number */
    uint64_t getMagicNumber ();
private:
    std::string content_;
};

#endif