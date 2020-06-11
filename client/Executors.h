#ifndef EXECUTORS_H
#define EXECUTORS_H

#include "Client.h"

namespace Executors {
    void connect_executor(Client* client, std::string& text);
    void ping_executor(Client* client, std::string& text);
    void nick_executor(Client* client, std::string& text);
    void quit_executor(Client* client, std::string& text);
    void say_executor(Client* client, std::string& text);
    void join_executor(Client* client, std::string& text);
}

#endif