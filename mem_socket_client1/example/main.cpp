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
    sWrite(sockfd, "1");
    sWrite(sockfd, name + " " + std::to_string(n * sizeof(T)));
    std::string ack = sRead(sockfd);
}

template<class T> T
mem_read(int sockfd, const std::string& name, int beg) {
    sWrite(sockfd, "2");
    sWrite(sockfd, name + " " + std::to_string(beg * sizeof(T)) + " " + std::to_string((beg + 1) * sizeof(T) - 1));

    // 获取字节序列
    uint8_t byteBuf[1024];
    if (recv(sockfd, &byteBuf, sizeof(byteBuf), 0) == -1) {
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
    std::string ack = sRead(sockfd);
    return ret;
}
template<class T> void
mem_write(int sockfd, const std::string& name, int beg, T value) {
    sWrite(sockfd, "3");
    sWrite(sockfd, name + " " + std::to_string(beg * sizeof(T)) + " " + std::to_string((beg + 1) * sizeof(T) - 1));

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
    if (send(sockfd, &byteBuf, sizeof(byteBuf), 0) == -1) {
        std::cout << "send error\n";
    }
    std::string ack = sRead(sockfd);
}

void mem_free (int sockfd, const std::string& name) {
    sWrite(sockfd, "4");
    sWrite(sockfd, name);
    std::string ack = sRead(sockfd);
    
    // READ(sockfd, buf);
    // std::cout << buf << std::endl;
}

struct node {
    int x, y;
    node (int _x = 0, int _y = 0) :
        x(_x),
        y(_y) {}
};

void sonWork (int id) {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 33);
    serv_sock->connect(serv_addr);
    std::string ack = sRead(serv_sock->getFd());
    std::cout << ack.size() << std::endl;

    sWrite(serv_sock->getFd(), "test" + std::to_string(id) + " " + "test" + std::to_string(id));
    ack = sRead(serv_sock->getFd());
     
    mem_ask(serv_sock->getFd(), "hello" + std::to_string(id), 100, (node*)nullptr);
    for (int i = 0; i < 100; i ++) {
        mem_write(serv_sock->getFd(), "hello" + std::to_string(id), i, node(i, i + 1));
    }
    for (int i = 0; i < 100; i ++) {
        node rd = mem_read<node>(serv_sock->getFd(), "hello" + std::to_string(id), i);
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << id << "->>: " << rd.x << " " << rd.y << std::endl;
    }

    sWrite(serv_sock->getFd(), "5");
    exit(0);
}

int main (int argc, char* argv[]) {
    sonWork(2);
    return 0;
}
