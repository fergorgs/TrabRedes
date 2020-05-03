# Internet Relay Chat - SSC0142
This repository is internet relay chat project for SSC0142 course (ICMC - USP).

## How to Run
**!!! THIS PROJECT HAS ONLY BEEN TESTED IN UBUNTU 18.04 & 19.10 & 20.04 !!!**  
**!!! COMPILER USED IS g++ (Ubuntu 9.3.0-10ubuntu2) 9.3.0, WITH c++11 !!!**

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

### Running
* 1st step: run the hub
    * It is important to run the hub first, given that the client needs to connect to the server (hub); if it isn't open, the client won't open either.
    ```
        $ ./hub
    ```

* 2nd & 3rd steps: run 2 client instances
    * After running the server (hub), you need to run 2 instances of the client mandatorily. As long as there are less than 2 client instances, the hub is stuck waiting for 2 connections. 
    ```
        $ ./client
        $ ./client
    ```

    * As soon as the client is ran, the program will ask for a nickname to identify the messages.
    * Then a window is open. In this window, on the top part all messages are shown and you can input your messages in the bottom left text area. To send the message, just click the "Send" button. As simple as that.

### Disclaimers
* When either instance of the client is closed, the hub shuts down as well. This is due to the way we made the hub, for 2 and only 2 connections.
* For now the threshold to break the message into parts is 4000 chars. This is due to addition of header data, such as the command used and user's nickname. In the next submissions, this size threshold may change to a bigger or smaller value, to avoid wasting space.


## Group Members
* Alexandre Galocha Pinto Junior (10734706) [git](https://github.com/alexandregjr)  
* Eduardo Pirro (10734665) [git](https://github.com/EdPirro)  
* Fernando Gorgulho Fayet (10734407) [git](https://github.com/fergorgs)