#include <fcntl.h>
#include <queue>
#include <stdio.h>
#include <sys/socket.h>

#include "Message.h"

class Connection {
    private:
        int socket;
        std::queue<Message*> q;
    public:
        Connection(int);
        ~Connection();
        void confirmReceive();
        void write(Message*);
        int read(Message*);
};