#include <fcntl.h>
#include <functional>
#include <queue>
#include <stdio.h>
#include <sys/socket.h>
#include <chrono>
#include <cstring>
#include <ctime>

#ifndef CONNECTION_H
#define CONNECTION_H

#include "MessageSendController.h"
#include "../utils/RFCprotocol.h"

class Connection {
    private:
        bool alive = true;
        int socket;
        int retryCount = 0;
        std::queue<MessageSendController*> q;
        std::chrono::time_point<std::chrono::system_clock> last_timestamp;
    public:
        std::string nick;
        Connection(int);
        ~Connection();
        void confirmReceive();
        void write(MessageSendController*);
        void pong();
        void send_msg(Message*);
        Message* read(int&);
};

#endif