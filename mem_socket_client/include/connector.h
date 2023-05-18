#pragma once

#include "socket.h"
#include "inet_address.h"
#include "util.h"
#include <string>
#include <cstring>

class Connector {
public:
    /* 默认构造：连接上服务端（ip:port 在配置文件 config.h 内） */
    Connector ();

    /* 带参构造：带已连接 socket */
    Connector (Socket* _serv_sock);

    /* 带参构造：带地址 */
    Connector (InetAddress* _inet);

    /* 析构：关闭连接 */
    ~Connector ();

    /* 登录服务端(用户名，密码) */
    bool Login (const std::string& username, const std::string& password);

    /* 变量申请(变量名, 变量个数（默认是1，代表单变量）) */
    template<typename T> void memAsk (const std::string& varname, int size = 1);

    /* 变量值修改(变量名, 修改后的值, 该变量是数组的第几个（默认为0，代表单变量）) */
    template<typename T> void memWrite (const std::string& varname, const T& v, int id = 0);

    /* 变量值获取(变量名, 该变量是数组的第几个（默认为0，代表单变量）)  return 获取到的值; */
    template<typename T> T memRead (const std::string& varname, int id = 0);

    /* 变量释放函数(变量名) */
    void memFree (const std::string& varname);

    /* 退出连接 */
    void Exit ();

private:
    Socket* serv_sock;  ///< 连接服务端的套接字
};


/**
 * @brief 变量申请
 * 
 * @tparam T 变量类型
 * @param varname 变量名
 * @param size    变量个数（默认是1，代表单变量）
 */
template<typename T> void 
Connector::memAsk (const std::string& varname, int size) {
    sWrite(serv_sock->getFd(), "1");
    sWrite(serv_sock->getFd(), varname + " " + std::to_string(size * sizeof(T)));
    std::string ack = sRead(serv_sock->getFd());
}

/**
 * @brief 变量值修改
 * 
 * @tparam T 变量类型
 * @param varname 变量名
 * @param value   修改后的值
 * @param id      该变量是数组的第几个（默认为0，代表单变量）
 */
template<typename T> void 
Connector::memWrite (const std::string& varname, const T& value, int id) {
    sWrite(serv_sock->getFd(), "3");
    sWrite(serv_sock->getFd(), varname + " " + std::to_string(id * sizeof(T)) + " " + std::to_string((id + 1) * sizeof(T) - 1));

    // 打造字节序列
    uint8_t byteBuf[128];
    bzero(&byteBuf, sizeof(byteBuf));
    T* ptr = new T(value);
    if (BitHighLow()) {
        uint8_t *onemem = (uint8_t *)ptr;
        for (int i = 0; i < sizeof(T); i ++) {
            byteBuf[i] = *onemem;
            onemem ++;
        }
    } else {
        uint8_t *onemem = (uint8_t *)(ptr + 1) - 1;
        for (int i = 0; i < sizeof(T); i ++) {
            byteBuf[i] = *onemem;
            onemem --;
        }
    }
    if (send(serv_sock->getFd(), byteBuf, sizeof(byteBuf), 0) == -1) {
        std::cout << "send error\n";
    }
    std::string ack = sRead(serv_sock->getFd());
}

/**
 * @brief 变量值获取
 * 
 * @tparam T 变量类型
 * @param varname 变量名
 * @param id      该变量的第几个位置（默认为0，代表单变量）
 * @return T 返回获取到的值
 */
template<typename T> T 
Connector::memRead (const std::string& varname, int id) {
    sWrite(serv_sock->getFd(), "2");
    sWrite(serv_sock->getFd(), varname + " " + std::to_string(id * sizeof(T)) + " " + std::to_string((id + 1) * sizeof(T) - 1));

    // 获取字节序列
    uint8_t byteBuf[128];
    if (recv(serv_sock->getFd(), byteBuf, sizeof(byteBuf), 0) == -1) {
        std::cout << "recv error\n";
    }
    T *ret_pointer = new T();
    if (BitHighLow()) {
        uint8_t *onemem = (uint8_t *)ret_pointer;
        for (int i = 0; i < sizeof(T); i ++) {
            *onemem = byteBuf[i];
            onemem ++;
        }
    } else {
        uint8_t *onemem = (uint8_t *)(ret_pointer + 1) - 1;
        for (int i = 0; i < sizeof(T); i ++) {
            *onemem = byteBuf[i];
            onemem --;
        }
    }
    T ret = *ret_pointer;
    delete ret_pointer;
    std::string ack = sRead(serv_sock->getFd());
    return ret;
}