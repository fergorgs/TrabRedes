# Internet Relay Chat - SSC0142
This repository is internet relay chat project for SSC0142 course (ICMC - USP).

## How to Run
**!!! THIS PROJECT HAS ONLY BEEN TESTED IN UBUNTU 18.04 & 19.10 & 20.04 !!!**

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

### Disclaimers
* When either instance of the client is closed, the hub shuts down as well. This is due to the way we made the hub, for 2 and only 2 connections.