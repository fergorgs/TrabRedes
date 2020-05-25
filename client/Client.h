#ifndef CLIENT_H
#define CLIENT_H

#include "../utils/RFCprotocol.h"

typedef void (* Hook)(class Client *, std::string&);

class Client {
    private:
        std::map<std::string, Hook> executors;
        std::map<std::string, Hook> handlers;

        int hub_socket;
    public:
        std::string nickname;
        bool connected;

        Client();
        
        void quit();
        void create_connection();
        void parse_command(std::string& str);
        
        bool receiver();

        void send_message(Message* msg);

        ~Client();
};

#endif