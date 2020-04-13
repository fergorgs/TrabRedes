#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
//#include <arpa/inet.h>

#define SERVER_DOOR 9030

using namespace std;

int main(){

    //define door (must be the same as Typer and Printer)
    cout << "State your door: ";

    int clientDoor;

    while(true){
        try
        {
            cin >> clientDoor;
            break;  
        }
        catch(const std::exception& e)
        {
            cout << "Invalid entry" << endl;
        }
    }

    //Commands to connect to hub
    cout << "Type 'y' to connect or 'n' to abort" << endl;

    while(true)
    {
        char resp;

        cin >> resp;

        if(resp == 'y')
            break;
        else if(resp == 'n')
            return 0;
    }
    

    //create a socket connection to the "Hub"
    //program
    int hubSocket = socket(AF_INET, SOCK_STREAM, 0);

    //setting the address struct to connect to "Hub"
    struct  sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

    //connect to "Hub"
    int connection_status = connect(hubSocket, (struct sockaddr*) &hub_address, sizeof(hub_address));

    //check for connection status
    if(connection_status != 0){
        cout << "Faild to connect to hub" << endl;
        return 0;
    }
    else
        cout << "Connected to hub" << endl;


    //create a socket to receive the "Typer"
    //and "Printer" connections
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    //setting the address struct to bind and way for printer and typer
    struct  sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(clientDoor);
    client_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to the specefied IP and port
    bind(clientSocket, (struct  sockaddr*) &client_address, sizeof(client_address));

    //client is listening
    listen(clientSocket, 2);

    //accepts the Typer and Printer connections, respectively
    int typerSocket = accept(clientSocket, NULL, NULL);
    if(typerSocket != -1)
        cout << "Connected to typer" << endl;
    else
        cout << "Failed to connect to typer" << endl;
    
    int printerSocket = accept(clientSocket, NULL, NULL);
    if(typerSocket != -1)
        cout << "Connected to printer" << endl;
    else
        cout << "Failed to connect to printer" << endl;


    //arrays to hold messeges sent by typer and receved from hub
    char sendingMessege[4096];
    char receivingMessege[4096];

    while(true)
    {
        //receives messeges from typer and sends them to hub
        const int result1 = recv(typerSocket, &sendingMessege, sizeof(sendingMessege), 0);
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
            if(strcmp(sendingMessege, "fim") == 0)
                break;

            send(hubSocket, sendingMessege, sizeof(sendingMessege), 0);
        }


        //receives messeges from hub and sends them to printer
        const int result2 = recv(hubSocket, &receivingMessege, sizeof(receivingMessege), 0);
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
            if(strcmp(receivingMessege, "fim") == 0)
                break;

            send(printerSocket, receivingMessege, sizeof(receivingMessege), 0);
        }
    }

    //shuts down both sockets with hub and typer and printer
    cout << "Client shutting down\n";

    shutdown(hubSocket, SHUT_RDWR);
    shutdown(clientSocket, SHUT_RDWR);
    
    return 0;
    
}