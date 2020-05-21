#include "Hub.h"

#define LOG 1

Hub::~Hub() {
    alive = false;
    shutdown(hubSocket, SHUT_RDWR);
    close(hubSocket);

    // if(LOG) std::cout << "HUB_LOG: Hub shutting down\n";

    exit(0);
}

void Hub::IOConnections() {
    while(alive) {
        auto it = connections.begin();
        while(it != connections.end()){
            Message* msg = new Message(connections.size());
            int flag = (*it)->read(msg);

            if(!flag) {
                delete msg;
                auto lit = it++;
                connections.erase(lit);
                continue;
            }

            if(flag == -1) {
                delete msg;
                if(errno != EAGAIN && errno != EWOULDBLOCK){
                    if(LOG) std::cout << "HUB_LOG: Error receiving messege from client " << errno << std::endl;
                }
            }
            else for(auto& s : connections) s->write(msg);
            ++it;
        }
    }
}

void Hub::waitConnection() {
    while(alive) {
        // accepts new connections while alive
        int nconn = accept(hubSocket, nullptr, nullptr);
        if(nconn != -1) {
            if(LOG) std::cout << "HUB_LOG: Connected to client" << std::endl;
            std::unique_ptr<Connection> ptr(new Connection(nconn));
            connections.push_back(std::move(ptr));
        } else if(LOG) std::cout << "HUB_LOG: Failed to connect to client" << std::endl;
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
    delete globalHub;
}


Hub::Hub() {

    globalHub = this;

    signal(SIGINT, wrapper);

    //create a socket to receive both
    //clients connections
    hubSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    // reuse port and addr for server
    if (setsockopt(hubSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
}
