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
#include <functional>

#include "Connection.h"
#include "Message.h"

#ifndef HUB_H
#define HUB_H

// typedef void (* Hook)(class Hub *, std::string&);

class Hub {
    private:
        int hubSocket = 0;
        bool alive;
        void waitConnection();
        void IOConnections();
        std::list<std::unique_ptr<Connection>> connections;
    public:
        Hub();
        void run(int);
        ~Hub();
        
};

#endif