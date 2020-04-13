#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

using namespace std;

int main(){

    //create a socket connection to the "Client"
    //program
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    //define door (must be the same as Client)
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

    
    
    //setting the address struct to connect to
    struct sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(clientDoor);
    client_address.sin_addr.s_addr = INADDR_ANY;

    //connect to "Client"
    int connection_status = connect(clientSocket, (struct sockaddr*) &client_address, sizeof(client_address));

    //check connection status
    if(connection_status != 0){
        cout << "Faild to connect\n";
        return 0;
    }

    //array to hold the response
    char clientResponse[4096];

    while (true)
    {
        //receives responses from Client
        //and handles the result of the request
        //Prints them on screen if everything works
        const int result = recv(clientSocket, &clientResponse, sizeof(clientResponse), 0);

        if(result == -1)
        {
            std::cout << "Error: " << errno << std::endl;
            break;
        }

        else if(result == 0)
        {
            std::cout << "Disconnected" << std::endl;
            break;
        }
        else
        {
            if(strcmp(clientResponse, "fim") == 0)
                break;

            cout << "Typer says: " << clientResponse << endl;
        }
    }


    //shuts down socket with client
    cout << "Printer shutting down" << endl;
    shutdown(clientSocket, SHUT_RDWR);
    return 0;
    
}