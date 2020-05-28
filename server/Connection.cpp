#include "Connection.h"
#include <iostream>

Message* Connection::read(int& ret) {
    auto now = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::seconds>(now -  last_timestamp).count();
    if(!q.empty() && dur > 1) {
        if(retryCount++ == 5) {
            ret = 0;
            return nullptr;
        }
        write(nullptr);
    }
    char m[4096];
    memset(m, 0, 4096);
    ret = recv(socket, m,  4096 * sizeof(char), 0);
    std::string s(m);
    Message* msg = nullptr;
    if(ret > 0) msg = new Message(std::ref(s));
    return msg;
}

void Connection::write(MessageSendController* msg) {
    if(msg) q.push(msg);
    if(!msg || q.size() == 1) {
        send(socket, q.front()->getBuffer(),  4096 * sizeof(char), 0);
        last_timestamp = std::chrono::system_clock::now();
    }
}

void Connection::confirmReceive() {
    retryCount = 0;
    q.front()->deduct();
    q.pop();
}

Connection::Connection(int s) : socket(s) {
    std::cout << "Connection constructed\n";
    fcntl(socket, F_SETFL, O_NONBLOCK);
}; 

void Connection::pong() {
    Message* ret = new Message();
    ret->command.setWord("pong");
    send(socket, ret->serializeMessage().c_str(),  4096 * sizeof(char), 0);
    delete ret;
}

void Connection::send_msg(Message* m) {
    send(socket, m->serializeMessage().c_str(),  4096 * sizeof(char), 0);
}

Connection::~Connection() {
    while(!q.empty()) q.front()->deduct(), q.pop();
    shutdown(socket, SHUT_RDWR);
    std::cout << "Connection destructed\n";
}
