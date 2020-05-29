#include "Hub.h"

#define LOG 1

Hub::~Hub() {
    cout << "HUB destructed";
    alive = false;
    shutdown(hubSocket, SHUT_RDWR);
    close(hubSocket);
    for(auto it = connections.begin(); it != connections.end(); ++it) {
        delete *it;
    }

    exit(0);
}

void Hub::IOConnections() {
    while(alive) {
        auto it = connections.begin();
        while(it != connections.end()){
            int flag;
            Message* msg = (*it)->read(flag);

            if(!flag) {
                if(msg) delete msg;
                auto lit = it++;
                nicks.erase((*lit)->nick);
                delete *lit;
                connections.erase(lit);
                continue;
            }

            if(flag <= 0) {
                if(errno != EAGAIN && errno != EWOULDBLOCK){
                    if(LOG) std::cout << "HUB_LOG: Error receiving messege from client " << errno << std::endl;
                }
            } else {
                handlers[msg->command.getWord()](msg, this, *it);
            }
            
            delete msg;
            ++it;
        }
    }
}

void Hub::waitConnection() {
    while(alive) {
        // accepts new connections while alive
        int nconn = accept(hubSocket, nullptr, nullptr);
        if(nconn > 0) {
            if(LOG) std::cout << "HUB_LOG: Connected to client" << std::endl;
            connections.push_back(new Connection(nconn));
        } else if(errno != EAGAIN && errno != EWOULDBLOCK && LOG) std::cout << "HUB_LOG: Failed to connect to client" << std::endl;
    }
}

void Hub::run(int port) {
    //setting the address struct to bind and wait for clients to connect
    struct  sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(port);
    hub_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to the specefied IP and port
    bind(hubSocket, (struct  sockaddr*) &hub_address, sizeof(hub_address));
    fcntl(hubSocket, F_SETFL, O_NONBLOCK);

    //Hub is listening
    listen(hubSocket, 40);
    alive = true;
    std::thread wc(&Hub::waitConnection, this);
    if (LOG) std::cout << "HUB_LOG: Waiting for connections..." << std::endl;
    std::thread io(&Hub::IOConnections, this);
    wc.join();
    io.join();
}

Hub* globalHub;


void wrapper(int) {
    globalHub->alive = false;
}


Hub::Hub() {

    globalHub = this;

    signal(SIGINT, wrapper);

    //create a socket to receive both
    //clients connections
    hubSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    handlers["say"] = Handlers::say;
    handlers["ping"] = Handlers::ping;
    handlers["nick"] = Handlers::nick;
    handlers["ack"] = Handlers::confirm;


    // reuse port and addr for server
    if (setsockopt(hubSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
}
