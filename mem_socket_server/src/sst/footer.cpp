#include "footer.h"
#include "utils.h"
#include <iostream>

/**
 * @brief 塞入编码了一半或者已有的字符串
 * 
 * @param content （半成品 or 成品）字节码
 */
FooterBuilder::FooterBuilder (const std::string& content) : content_(content) {}

/**
 * @brief 编码偏移量和大小
 * 
 * @param offset 偏移量
 * @param size 大小
 */
void FooterBuilder::addIndexBlock(uint64_t offset, uint64_t size) {
    setFixed64(&content_, offset);
    setFixed64(&content_, size);
}

/**
 * @brief 用 magic_number 的字节形式封底
 * 
 * @param magic_number 封底的检查数
 */
void FooterBuilder::finish(uint64_t magic_number) {
    setFixed64(&content_, magic_number);
}

/**
 * @brief 将编码后的 footer 刷到字符串 dst 上（调用前应确保调用了 finish）
 * 
 * @param dst 作为合入对象的字符串
 */
void FooterBuilder::flush(std::string* dst) {
    dst->append(content_);
}

// ------------------------ Footer

/**
 * @brief 构造：解析已有字符串
 * 
 * @param content 待解析字符串
 */
Footer::Footer(const std::string &content) : content_(content) {}


/**
 * @brief 构造：解析已有字符串
 * 
 * @param content 待解析字符串
 */
Footer::Footer(const char *content) : content_(content) {}

/**
 * @brief 当前 offset 仅能 0 ，用 footer 通过 footer 内的偏移量 offset 获取 Table 内 indexBlock 的绝对偏移量和大小
 * 
 * @param offset block_info 的偏移量
 * @return std::pair<uint64_t, uint64_t> 
 *     @retval first： 块在 table的字节码 中的偏移量
 *     @retval second： 块的长度
 */
std::pair<uint64_t, uint64_t> Footer::getBlock(uint64_t offset) {
    return std::pair<uint64_t, uint64_t>(
        getFixed64(content_.substr(offset, 8)), 
        getFixed64(content_.substr(offset + 8, 8))
    );
}

/**
 * @brief 解析出魔数的值
 * 
 * @return uint64_t magic_number
 */
uint64_t Footer::getMagicNumber() {
    return getFixed64(content_.substr(content_.size() - 8, 8));
}
