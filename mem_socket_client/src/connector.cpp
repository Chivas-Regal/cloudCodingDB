#include "connector.h"
#include "config.h"
#include "util.h"

Connector::Connector() {
    InetAddress* serv_addr = new InetAddress(SERVER_IP, SERVER_PORT);
    serv_sock = new Socket();
    serv_sock->connect(serv_addr);
    std::string ack = sRead(serv_sock->getFd());
}

Connector::Connector(Socket* _serv_sock) :
    serv_sock(_serv_sock)
{}

Connector::Connector(InetAddress* _inet) {
    serv_sock = new Socket();
    serv_sock->connect(_inet);
    std::string ack = sRead(serv_sock->getFd());
}

Connector::~Connector() {
    if (serv_sock != nullptr) {
        delete serv_sock;
        serv_sock = nullptr;
    }
}

bool Connector::Login (const std::string& username, const std::string& password) {
    sWrite(serv_sock->getFd(), username + " " + password);
    std::string ack = sRead(serv_sock->getFd());
    if (ack != "OK") {
        return false;
    }
    return true;
}

void Connector::memFree (const std::string& varname) {
    sWrite(serv_sock->getFd(), "4");
    sWrite(serv_sock->getFd(), varname);
    std::string ack = sRead(serv_sock->getFd());
}

void Connector::Exit () {
    sWrite(serv_sock->getFd(), "5");
}