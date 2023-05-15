#pragma once

#include "socket.h"
#include "inet_address.h"
#include "util.h"
#include <string>
#include <cstring>

class Connector {
public:
    Connector ();
    Connector (Socket* _serv_sock);
    Connector (InetAddress* _inet);
    ~Connector ();

    bool Login (const std::string& username, const std::string& password);
    template<typename T> void memAsk (const std::string& varname, int size);
    template<typename T> void memWrite (const std::string& varname, const T& v, int id = 0);
    template<typename T> T memRead (const std::string& varname, int id);
    void memFree (const std::string& varname);
    void Exit ();

private:
    Socket* serv_sock;
};


template<typename T> void 
Connector::memAsk (const std::string& varname, int size) {
    sWrite(serv_sock->getFd(), "1");
    sWrite(serv_sock->getFd(), varname + " " + std::to_string(size * sizeof(T)));
    std::string ack = sRead(serv_sock->getFd());
}

template<typename T> void 
Connector::memWrite (const std::string& varname, int id, const T& value) {
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