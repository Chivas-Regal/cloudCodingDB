# include <sys/socket.h>
# include <arpa/inet.h>
# include <string>
# include <unistd.h>
# include <iostream>
# include <map>
#include <cstring>
#include <thread>

#include "../include/util.h"
#include "../include/socket.h"
// client

template<class T> void
mem_ask (int sockfd, const std::string& name, int n, T*) {
    std::string buf;
    // 传操作序号
    buf = "1";
    sWrite(sockfd, buf);
    // 传变量名
    buf = name;
    sWrite(sockfd, buf);
    // 传要获得的字节数
    buf = std::to_string(n * sizeof(T));
    sWrite(sockfd, buf);
}

template<class T> T
mem_read(int sockfd, const std::string& name, int beg) {
    std::string buf;
    // 传操作号
    buf = "2";
    sWrite(sockfd, buf);
    // 传变量名
    buf = name;
    sWrite(sockfd, buf);
    // 传开始位置偏移的字节数
    buf = std::to_string(beg * sizeof(T));
    sWrite(sockfd, buf);
    // 传结束位置偏移的字节数
    buf = std::to_string((beg + 1) * sizeof(T) - 1);
    sWrite(sockfd, buf);

    // 获取字节序列
    uint8_t byteBuf[1024];
    if (recv(sockfd, byteBuf, 1024, 0) == -1) {
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
    return ret;
}
template<class T> void
mem_write(int sockfd, const std::string& name, int beg, T value) {
    std::string buf;
    // 传操作号
    buf = "3";
    sWrite(sockfd, buf);
    // 传变量名
    buf = name;
    sWrite(sockfd, buf);
    // 传开始位置偏移的字节数
    buf = std::to_string(beg * sizeof(T));
    sWrite(sockfd, buf);
    // 传结束位置偏移的字节数
    buf = std::to_string((beg + 1) * sizeof(T) - 1);
    sWrite(sockfd, buf);

    // 打造字节序列
    uint8_t byteBuf[1024];
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
    if (send(sockfd, byteBuf, 1024, 0) == -1) {
        std::cout << "send error\n";
    }
}

void mem_free (int sockfd, const std::string& name) {
   std::string buf;
    // 传操作号
    buf = "4";
    sWrite(sockfd, buf);
    // 传变量名
    buf = name;
    sWrite(sockfd, buf);
    
    // READ(sockfd, buf);
    // std::cout << buf << std::endl;
}

struct node {
    int x, y;
    node (int _x = 0, int _y = 0) :
        x(_x),
        y(_y) {}
};

int main () {
    std::string s;
    for (int i = 0; i < 100; i ++) s += '\0';
    std::cout << s << " " << s.size() << std::endl;
    return 0;
    
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 7777);
    serv_sock->connect(serv_addr);

    sWrite(serv_sock->getFd(), "123");
    sWrite(serv_sock->getFd(), "123");

    mem_ask(serv_sock->getFd(), "hello1", 100, (node*)nullptr);
    for (int i = 0; i < 100; i ++) {
        mem_write(serv_sock->getFd(), "hello1", i, node(i, i + 1));
    }
    for (int i = 0; i < 100; i ++) {
        node rd = mem_read<node>(serv_sock->getFd(), "hello1", i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << rd.x << " " << rd.y << std::endl;
    }
    return 0;
}

/*
int main () {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr; bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7777);

    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    mem_ask("hello", 10, (int*)nullptr);
    for (int i = 0; i < 10; i ++) {
        mem_write("hello", i, i);
    }
    for (int i = 0; i < 10; i ++) {
        std::cout << mem_read<int>("hello", i) << std::endl;
    }
}

*/