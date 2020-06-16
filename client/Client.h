#ifndef CLIENT_H
#define CLIENT_H

#include "../utils/RFCprotocol.h"

typedef void (* ExecutorHook)(class Client *, std::string&);
typedef void (* HandlerHook)(class Client *, class Message*);

class Client {
    private:
        std::map<std::string, ExecutorHook> executors;
        std::map<std::string, HandlerHook> handlers;

        int hub_socket;
    public:
        std::string nickname;
        std::string channel;
        std::chrono::steady_clock::time_point sentTime;
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