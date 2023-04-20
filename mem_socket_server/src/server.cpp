#include "../include/server.h"
#include "../include/util.h"
#include "../include/config.h"

#include <string>
#include <cstring>
#include <iostream>

Server::Server(EventLoop *_loop) :
    loop(_loop) 
{
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress(SERVER_IP, SERVER_PORT);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Channel *serv_channel = new Channel(loop, serv_sock->getFd());
    std::function<void()> _cb = std::bind(&Server::handleConnectionEvent, this, serv_sock);
    serv_channel->setCallback(_cb);
    serv_channel->enableReading();
}

Server::~Server () {
    delete loop;
}

void Server::handleConnectionEvent(Socket *serv_sock) {
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));
    std::cout << "new client fd " << clnt_sock->getFd() << std::endl;
    Channel *clnt_channel = new Channel(loop, clnt_sock->getFd());
    std::function<void()> _cb = std::bind(&Server::handleLoginEvent, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

void Server::handleLoginEvent (Channel* clnt_channel) {
    std::string username = sRead(clnt_channel->getFd());
    std::string password = sRead(clnt_channel->getFd());
    // std::cout << username << " " << password << std::endl;
    if (loop->sqm->QueryGetRes("SELECT * FROM mem_socket_login_infos WHERE username = '" + username + "' and password = '" + password + "';").empty()) { // ERROR: username or password not found
        close(clnt_channel->getFd()); 
        std::cout << "error login\n";
        return;
    } else {
        std::cout << "successfully login\n";
    }
    clnt_channel->setName(username);
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

void Server::handleTaskChoose(Channel* clnt_channel) {
    std::cout << "handlleTaskChoose: ";
    std::string taskId = sRead(clnt_channel->getFd());
    std::cout << taskId << "\n";
    std::function<void()> _cb;
    if (taskId == "1") {
        _cb = std::bind(&Server::handleNewMemplace, this, clnt_channel);
    } else if (taskId == "2") {
        _cb = std::bind(&Server::handleInputValue, this, clnt_channel);
    } else if (taskId == "3") {
        _cb = std::bind(&Server::handleOutputValue, this, clnt_channel);
    } else if (taskId == "4") {
        _cb = std::bind(&Server::handleFreeMemplace, this, clnt_channel);
    } else { // ERROR: taskid not found
        close(clnt_channel->getFd());
        return;
    }
    sWrite(clnt_channel->getFd(), "OK");
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

void Server::handleNewMemplace(Channel* clnt_channel) {
    std::string name = sRead(clnt_channel->getFd());
    std::string nBytes = sRead(clnt_channel->getFd());
    
    loop->opeMemKV(
        [&](){
            loop->memKV[clnt_channel->getName()][name] = std::make_pair<uint8_t*, size_t>(
                static_cast<uint8_t*>(loop->mempool->allocate(std::stoi(nBytes))),
                static_cast<size_t>(std::stoi(nBytes))
            );
        }
    );

    sWrite(clnt_channel->getFd(), "OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

void Server::handleInputValue(Channel* clnt_channel) {
    std::string name = sRead(clnt_channel->getFd());
    std::string _sstart = sRead(clnt_channel->getFd());
    std::string _send = sRead(clnt_channel->getFd());
    std::cout << name << " " << _sstart << " " << _send << std::endl;
    uint8_t* start = loop->memKV[clnt_channel->getName()][name].first + std::stoi(_sstart);
    uint8_t* end =   loop->memKV[clnt_channel->getName()][name].first + std::stoi(_send);
    uint8_t byteBuf[1024]; bzero(byteBuf, sizeof(byteBuf));
    if (BitHighLow()) {
        uint8_t* onemem = start;
        for (int i = 0; onemem - 1 != end; i ++) {
            byteBuf[i] = *onemem;
            onemem ++;
        }
    } else {
        uint8_t* onemem = end;
        for (int i = 0; onemem + 1 != start; i ++) {
            byteBuf[i] = *onemem;
            onemem --;
        }
    }
    if (send(clnt_channel->getFd(), byteBuf, 1024, 0) == -1) {
        std::cout << "send error\n";
    }
    sWrite(clnt_channel->getFd(), "OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

void Server::handleOutputValue(Channel* clnt_channel) {
    std::string name = sRead(clnt_channel->getFd());
    std::string _sstart = sRead(clnt_channel->getFd());
    std::string _send = sRead(clnt_channel->getFd());
    uint8_t* start = loop->memKV[clnt_channel->getName()][name].first + std::stoi(_sstart);
    uint8_t* end =   loop->memKV[clnt_channel->getName()][name].first + std::stoi(_send);

    uint8_t byteBuf[1024];
    bzero(&byteBuf, sizeof(byteBuf));
    if (recv(clnt_channel->getFd(), byteBuf, 1024, 0) == -1) {
        std::cout << "recv error\n";
    }

    if (BitHighLow()) {
        uint8_t* onemem = start;
        for (int i = 0; onemem - 1 != end; i ++) {
            *onemem = byteBuf[i];
            onemem ++;
        }
    } else {
        uint8_t* onemem = end;
        for (int i = 0; onemem + 1 != start; i ++) {
            *onemem = byteBuf[i];
            onemem --;
        }
    }
    sWrite(clnt_channel->getFd(), "OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}

void Server::handleFreeMemplace(Channel* clnt_channel) {
    std::string name = sRead(clnt_channel->getFd());
    loop->mempool->deallocate(
        loop->memKV[clnt_channel->getName()][name].first, 
        loop->memKV[clnt_channel->getName()][name].second
    );
    loop->opeMemKV(
        [&](){
            loop->memKV[clnt_channel->getName()].erase(name);
        }
    );
    sWrite(clnt_channel->getFd(), "OK");
    std::function<void()> _cb = std::bind(&Server::handleTaskChoose, this, clnt_channel);
    clnt_channel->setCallback(_cb);
    clnt_channel->enableReading();
}