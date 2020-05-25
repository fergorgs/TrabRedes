#include "Connection.h"

#include <iostream>


int Connection::read(Message* msg) {
    char m[4096];
    int ret = recv(socket, m,  4096 * sizeof(char), 0);
    if(ret > 0) msg->setBuffer(m);
    return ret;
}

void Connection::write(Message* msg) {
    q.push(msg);
    send(socket, q.front()->getBuffer(),  4096 * sizeof(char), 0);
    q.pop();
}

void Connection::confirmReceive() {
    q.front()->deduct();
    q.pop();
}

Connection::Connection(int s) : socket(s) {
    std::cout << "Connection constructed\n";
    fcntl(socket, F_SETFL, O_NONBLOCK);
}; 

Connection::~Connection() {
    while(!q.empty()) q.front()->deduct(), q.pop();
    std::cout << "Connection destructed\n";
}