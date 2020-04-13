#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
//#include <arpa/inet.h>

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

    //check for connection status
    if(connection_status != 0){
        cout << "Faild to connect\n";
        return 0;
    }


    //array to hold the messege to be sent
    char typerMessege[4096] = "Works";

    while (true)
    {
        //scans for messeges from typer and sends them to "Client"
        cin.getline(typerMessege, sizeof(typerMessege));

        send(clientSocket, typerMessege, sizeof(typerMessege), 0);

        if(strcmp(typerMessege, "fim") == 0)
            break;
    }


    //shuts down socket with client
    cout << "Typer shutting down\n";
    shutdown(clientSocket, SHUT_RDWR);

    return 0;
    
}