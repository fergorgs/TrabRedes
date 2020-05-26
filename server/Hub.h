#ifndef HUB_H
#define HUB_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <list>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
#include <memory>
#include <unordered_map>
#include <list>
#include <string>
#include <functional>

#include "Connection.h"
#include "MessageSendController.h"
#include "../utils/RFCprotocol.h"
#include "Handlers.h"

// typedef void (* Hook)(class Hub *, std::string&);

class Hub {
    private:
        int hubSocket = 0;
        bool alive;
        void waitConnection();
        void IOConnections();
        std::unordered_map<string, std::function<void(Message*, Hub*, Connection*)>> handlers;
    public:
        std::list<Connection*> connections;
        std::unordered_map<std::string, Connection*> nicks;
        Hub();
        void run(int);
        ~Hub();
        
};

#endif