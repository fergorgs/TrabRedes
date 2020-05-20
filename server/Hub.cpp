#include "Hub.h"

#define LOG 1;

Hub::~Hub() {
    alive = false;
    shutdown(hubSocket, SHUT_RDWR);
    close(hubSocket);

    if (LOG) cout << "HUB_LOG: Hub shutting down\n";

    exit(0);
}

void Hub::IOConnections() {
    char msg[4096];
    memset(msg, 0, 4096);
    while(alive) {
        auto it = connections.begin();
        while(it != connections.end()){
            int flag = c.read(msg);
            if(!flag) {
                auto lit = it;
                ++it;
                connections.erase(lit);
                continue;
            }

            if(flag == -1) {
                if(errno != EAGAIN && errno != EWOULDBLOCK){
                    if(LOG) std::cout << "HUB_LOG: Error receiving messege from client " << errno << std::endl;
                    break;
                }
            }
            else for(auto s : connections) s.send(msg);
            ++it;

            //receives messeges sent from client1 and sends them to client2
            // const int res = recv(clientSocket_1, &client1Messege, sizeof(client1Messege), 0);
            // if(result1 == -1)
            // {
            //     i
            // }

            // else if(result1 == 0)
            // {
            //     if(LOG) std::cout << "HUB_LOG: Disconnected from client1" << std::endl;
            //     break;
            // }
            // else
            // {
            //     if(strcmp(client1Messege, "/quit") == 0){
            //         if(LOG) cout << "HUB_LOG: Client1 ordered hub to shut down" << endl;
            //         break;
            //     }

            //     if(LOG) cout << "HUB_LOG: Received messege from client1:" << endl;
            //     if(LOG) cout << "\t'" << client1Messege << "'" << endl;

            //     send(clientSocket_2, client1Messege, sizeof(client1Messege), 0);
            //     send(clientSocket_1, client1Messege, sizeof(client1Messege), 0);
            // }
        }
    }
}

void Hub::waitConnection() {
    while(alive) {
        // accepts new connections while alive
        int nconn = accept(hubSocket, nullptr, nullptr);
        if(nconn != -1)
            if(LOG) cout << "HUB_LOG: Connected to client" << endl;
            connections.push(Connection(nconn));
        else
            if(LOG) cout << "HUB_LOG: Failed to connect to client" << endl;
    }
}

Hub::Hub() {

    signal(SIGINT, Hub::~Hub);

    //create a socket to receive both
    //clients connections
    hubSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    // reuse port and addr for server
    if (setsockopt(hubSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }

    //setting the address struct to bind and way for clients to connect
    struct  sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to the specefied IP and port
    bind(hubSocket, (struct  sockaddr*) &hub_address, sizeof(hub_address));

    //Hub is listening
    listen(hubSocket, 2);
    alive = true;
    std::thread(waitConnection);
    if (LOG) cout << "HUB_LOG: Waiting for connections..." << endl;
    std::thread(IOConnection);

    while(alive) continue;

    
    
    if (LOG) cout << "HUB_LOG: Hub shutting down\n";
    
    return 0;
}
