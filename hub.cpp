#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define SERVER_DOOR 9030

using namespace std;

int main(){

    //create a socket to receive both
    //clients connections
    int hubSocket = socket(AF_INET, SOCK_STREAM, 0);

    //setting the address struct to bind and way for clients to connect
    struct  sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to the specefied IP and port
    bind(hubSocket, (struct  sockaddr*) &hub_address, sizeof(hub_address));

    //Hub is listening
    listen(hubSocket, 2);

    //accepts client1 and client2 connections, respectively
    int clientSocket_1 = accept(hubSocket, NULL, NULL);
    if(clientSocket_1 != -1)
        cout << "Connected to client 1" << endl;
    else
        cout << "Failed to connect to client 1" << endl;
    
    int clientSocket_2 = accept(hubSocket, NULL, NULL);
    if(clientSocket_2 != -1)
        cout << "Connected to client 2" << endl;
    else
        cout << "Failed to connect to client 2" << endl;

    //arrays to hold messeges sent by client1 to client2 and vice-versa
    char client1Messege[4096];
    char client2Messege[4096];

    while(true)
    {
        //receives messeges sent from client1 and sends them to client2
        const int result1 = recv(clientSocket_1, &client1Messege, sizeof(client1Messege), 0);
        if(result1 == -1)
        {
            std::cout << "Error: " << errno << std::endl;
            break;
        }

        else if(result1 == 0)
        {
            std::cout << "Disconnected" << std::endl;
            break;
        }
        else
        {
            if(strcmp(client1Messege, "fim") == 0)
                break;

            send(clientSocket_2, client1Messege, sizeof(client1Messege), 0);
        }


        //receives messeges sent from client2 and sends them to client1
        const int result2 = recv(clientSocket_2, &client2Messege, sizeof(client2Messege), 0);
        if(result2 == -1)
        {
            std::cout << "Error: " << errno << std::endl;
            break;
        }

        else if(result2 == 0)
        {
            std::cout << "Disconnected" << std::endl;
            break;
        }
        else
        {
            if(strcmp(client2Messege, "fim") == 0)
                break;

            send(clientSocket_1, client2Messege, sizeof(client2Messege), 0);
        }
    }

    //shuts down the hub socket
    cout << "Hub shutting down\n";

    shutdown(hubSocket, SHUT_RDWR);
    
    return 0;
        
}