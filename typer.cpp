#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>
//#include <arpa/inet.h>

#define LOG 0

using namespace std;

static volatile int *clientSocketAddr = NULL;

void shutDown(int dummy){
    
    cout << endl << "Typer shutting down\n";
    shutdown(*clientSocketAddr, SHUT_RDWR);

    exit(0);

}

int main(){

    signal(SIGINT, shutDown);

    //create a socket connection to the "Client"
    //program
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    clientSocketAddr = &clientSocket;

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
        if(LOG) cout << "TYPER_LOG: Faild to connect\n";
        return 0;
    }
    else
        if(LOG) cout << "TYPER_LOG: Connected to client" << endl;
    


    //array to hold the messege to be sent
    char typerMessege[4096] = "Works";

    while (true)
    {
        //scans for messeges from typer and sends them to "Client"
        cin.getline(typerMessege, sizeof(typerMessege));

        send(clientSocket, typerMessege, sizeof(typerMessege), 0);

        if(strcmp(typerMessege, "/quit") == 0){
               if(LOG) cout << "TYPER_LOG: User ordered typer to shut down" << endl;
               break;
        }
    }


    //shuts down socket with client
    cout << "Typer shutting down\n";
    shutdown(clientSocket, SHUT_RDWR);

    return 0;
    
}