#include <stdio.h>
#include <stdlib.h>
//#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>

#include "RFCprotocol.h"
//#include <arpa/inet.h>

#define SERVER_DOOR 9030
#define LOG 1

using namespace std;

static volatile int *hubSocketAddr = NULL;
static volatile int *clientSocketAddr = NULL;

void shutDown(int dummy){
    
    cout << endl << "Client shutting down\n";
    shutdown(*hubSocketAddr, SHUT_RDWR);
    shutdown(*clientSocketAddr, SHUT_RDWR);

    exit(0);

}

int main(){

    signal(SIGINT, shutDown);

    //define door (must be the same as Typer and Printer)
    int clientDoor;
    string nick;

    cout << "State your door: ";

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

    cout << "Choose a nick name: ";

    cin >> nick;

    //create a socket connection to the "Hub"
    //program
    int hubSocket = socket(AF_INET, SOCK_STREAM, 0);

    hubSocketAddr = &hubSocket;

    //setting the address struct to connect to "Hub"
    struct  sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

    //connect to "Hub"
    int connection_status = connect(hubSocket, (struct sockaddr*) &hub_address, sizeof(hub_address));

    //check for connection status
    if(connection_status != 0){
        if(LOG) cout << "CLIENT_LOG: Faild to connect to hub" << endl;
        return 0;
    }
    else
        if(LOG) cout << "CLIENT_LOG: Connected to hub" << endl;


    //create a socket to receive the "Typer"
    //and "Printer" connections
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    clientSocketAddr = &clientSocket;

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
        if(LOG) cout << "CLIENT_LOG: Connected to typer" << endl;
    else
        if(LOG) cout << "CLIENT_LOG: Failed to connect to typer" << endl;
    
    int printerSocket = accept(clientSocket, NULL, NULL);
    if(typerSocket != -1)
        if(LOG) cout << "CLIENT_LOG: Connected to printer" << endl;
    else
        if(LOG) cout << "CLIENT_LOG: Failed to connect to printer" << endl;

    //makes the socket non binding
    fcntl(typerSocket, F_SETFL, O_NONBLOCK);
    fcntl(printerSocket, F_SETFL, O_NONBLOCK);
    fcntl(hubSocket, F_SETFL, O_NONBLOCK);


    //arrays to hold messeges sent by typer and receved from hub
    char sendingMessege[4096];
    char receivingMessege[4096];

    int lastResult1 = -1;
    int lastResult2 = -1;

    while(true)
    {
        //receives messeges from typer and sends them to hub
        const int result1 = recv(typerSocket, &sendingMessege, sizeof(sendingMessege), 0);

        if(result1 == -1)
        {
            if(errno != EAGAIN && errno != EWOULDBLOCK){
                if(LOG) cout << "CLIENT_LOG: Error receiving messege from typer: " << errno << std::endl;
                break;
            }
        }

        else if(result1 == 0)
        {
            if(LOG) cout << "CLIENT_LOG: Disconnected from typer" << std::endl;
            break;
        }
        else
        {
            if(sendingMessege[0] != 0)
            {
                if(LOG) cout << "CLIENT_LOG: Received messege from typer:" << endl;
                if(LOG) cout << "\t'" << sendingMessege << "'" << endl;

                Messege msg = Messege();

                msg.prefix.setNick(nick);
                msg.command.setWord("/say");
                msg.params.setTrailing(sendingMessege);

                if(LOG) cout << "\t DEBUG DEBUG: ||" << msg.params.getTrailing() << "||" << endl;
                
                char serldMsg[4096];
                
                strcpy(serldMsg, msg.serializeMessege().c_str());

                send(hubSocket, serldMsg, sizeof(serldMsg), 0);
            }


            if(strcmp(sendingMessege, "/quit") == 0){
                if(LOG) cout << "CLIENT_LOG: Typer ordered client to shut down" << endl;
                break;
            }
        }

        //receives messeges from hub and sends them to printer
        const int result2 = recv(hubSocket, &receivingMessege, sizeof(receivingMessege), 0);

        if(result2 == -1)
        {
            if(errno != EAGAIN && errno != EWOULDBLOCK){
                if(LOG) cout << "CLIENT_LOG: Error receiving messege from hub: " << errno << std::endl;
                break;
            }
        }

        else if(result2 == 0)
        {
            if(LOG) cout << "CLIENT_LOG: Disconnected from hub" << std::endl;
            break;
        }
        else
        {
            if(LOG) cout << "CLIENT_LOG: Received messege from hub:" << endl;
            if(LOG) cout << "\t'" << receivingMessege << "'" << endl;

            Messege msg = Messege(receivingMessege);

            char deSerldMsg[4096];

            strcpy(deSerldMsg, msg.params.getTrailing().c_str());

            send(printerSocket, deSerldMsg, sizeof(deSerldMsg), 0);

            if(strcmp(sendingMessege, "/quit") == 0){
                if(LOG) cout << "CLIENT_LOG: Hub ordered client to shut down" << endl;
                break;
            }
        }
    }

    //shuts down both sockets with hub and typer and printer
    cout << "Client shutting down\n";

    shutdown(hubSocket, SHUT_RDWR);
    shutdown(clientSocket, SHUT_RDWR);
    
    return 0;
    
}