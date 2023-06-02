#include "connector.h"
#include "config.h"
#include "util.h"
#include <sstream>

/**
 * @brief 默认构造函数
 * 
 * @details 连接上服务端（ip:port 在配置文件 config.h 内）
 *          获取到一个 ack 字符串
 */
Connector::Connector() {
    InetAddress* serv_addr = new InetAddress(SERVER_IP, SERVER_PORT);
    serv_sock = new Socket();
    serv_sock->connect(serv_addr);
    std::string ack = sRead(serv_sock->getFd());
}

/**
 * @brief 带已连接 socket 构造函数
 * 
 * @param _serv_sock 已连接 socket 
 */
Connector::Connector(Socket* _serv_sock) :
    serv_sock(_serv_sock)
{}

/**
 * @brief 带地址构造函数
 * 
 * @param _inet 地址
 */
Connector::Connector(InetAddress* _inet) {
    serv_sock = new Socket();
    serv_sock->connect(_inet);
    std::string ack = sRead(serv_sock->getFd());
}

/**
 * @brief 析构函数
 * 
 * @details 关闭连接
 */
Connector::~Connector() {
    if (serv_sock != nullptr) {
        delete serv_sock;   // serv_sock 的析构会自己 delete 的
        serv_sock = nullptr;
    }
}

/**
 * @brief 登录服务端
 * 
 * @param username 用户名
 * @param password 密码
 * 
 * @return true    登陆成功
 * @return false   登陆失败
 * 
 * @details 向服务端发送 username + " " + password 
 *          根据收到的 ack 判断是否为 OK 来确定是否登录成功
 */
bool Connector::Login (const std::string& username, const std::string& password) {
    sWrite(serv_sock->getFd(), toDBsentense(username + " " + password));
    std::string ack = sRead(serv_sock->getFd());
    if (ack != "OK") {
        return false;
    }
    return true;
}

/**
 * @brief 变量释放函数
 * 
 * @param varname 变量名
 * 
 * @details 发送 "4" 和 变量名，收取一个 ACK
 */
void Connector::memFree (const std::string& varname) {
    sWrite(serv_sock->getFd(), toDBsentense("DES " + varname));
    std::string ack = sRead(serv_sock->getFd());
}

/**
 * @brief 退出连接
 */
void Connector::Exit () {
    sWrite(serv_sock->getFd(), "5");
}

/**
 * @brief 获取格式化后的 db 语句
 *      a abc dd
 *   -> #3\r\n
 *   -> $1 a\r\n 
 *   -> $3 abc\r\n 
 *   -> $2 dd\r\n 
*/
std::string Connector::toDBsentense(const std::string& s) {
    std::stringstream sst;
    sst << s;
    int n = 0;
    std::string word, words;
    while (sst >> word) {
        words += "$" + std::to_string(word.size()) + " " + word + "\r\n";
        n ++;
    }
    return "#" + std::to_string(n) + "\r\n" + words;
}