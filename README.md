# Internet Relay Chat - SSC0142
This repository is internet relay chat project for SSC0142 course (ICMC - USP).

## How to Run
**!!! THIS PROJECT HAS ONLY BEEN TESTED IN UBUNTU 18.04 & 19.10 & 20.04 !!!**  
**!!! COMPILER USED IS g++ (Ubuntu 9.3.0-10ubuntu2) 9.3.0, WITH c++17 !!!**
**!!! THE PROGRAM HAS USER INTERFACE, SO DOESN'T RUN ON INTERFACELESS OS !!!**

### Compilation
* 1st step: Install gtkmm lib on your OS
    * This can be easily acheived by running the following command:
    ```
        $ apt install libgtkmm-3.0-dev
    ```

* 2nd step: compile all programs
    * That is easier than the first step, because we prepared a Makefile, so you just need to run:
    ```
        $ make all
    ```

### Running Client
* To run the client, simply type:
    ```
        $ make client
    ```

    * Note: it is no longer necessary to run the server program first. The server must be running only to connect the client program.

* **Implemented Client Commands**  
Once the Client is running, you have the following commands at your disposal:
    * **/connect**: use this command to connect to the server. It will only work if the server program is running. You must be connected to execute all other commands (except /quit, of course).
    * **/quit**: this command terminates the application. It has the same effect as clicking on the famous red 'x' in the top window bar.
    * **/nick**: being connected to the server, this command makes you define or change your nickname. If you've just connected, your nickname isn't defined and you must define your nickname to be able to send messages in chat.
    * **/ping**: this command sends a "PING" to the server and waits for "PONG" response. It is used to measure the socket latency. You need to be connected to do this.

### Running Server
* To run the server application, simply:
    ```
        $ make server
    ```
* The server application doesn't interact with the user. It only logs some messages of connections being made or broken, and message management.  
* To close this application the user just have to send an interrupt (SIGINT, Ctrl-C) via terminal.

### Disclaimers
* The server now can handle more than 2 connections and doesn't close all applications on shutdown, just disconnects all client applications. Then, the client can connect only after the server is running again.
* For now the threshold to break the message into parts is 4000 chars. This is due to addition of header data, such as the command used and user's nickname. In the next submissions, this size threshold may change to a bigger or smaller value, to avoid wasting space.


## Group Members
* Alexandre Galocha Pinto Junior (10734706) [git](https://github.com/alexandregjr)  
* Eduardo Pirro (10734665) [git](https://github.com/EdPirro)  
* Fernando Gorgulho Fayet (10734407) [git](https://github.com/fergorgs)