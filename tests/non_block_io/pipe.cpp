#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>

#define MAX_MSG 4096

void parent(int* read_fp, int* write_fp) {
    int res;
    char buff[4096];

    close(write_fp[0]);
    close(read_fp[1]);

    int count = 27;
    while (true) {
        res = read(read_fp[0], buff, 4096);

        if (res == -1)
            std::cout << "(empty)" << std::endl, usleep(100);
        else if (res == 0)
            std::cout << "(end)" << std::endl;
        else
            std::cout << "Client: " << buff << std::endl;

        if (count)
            write(write_fp[1], "HELLO FROM SERVER", MAX_MSG), count--;
        else 
            exit(0);
    }
}

void child(int* read_fp, int* write_fp) {
    int res;
    char buff[4096];

    close(write_fp[0]);
    close(read_fp[1]);

    int count = 10;
    while (true) {
        res = read(read_fp[0], buff, 4096);

        if (res == -1)
            std::cout << "(empty)" << std::endl, usleep(200);
        else if (res == 0)
            std::cout << "(end)" << std::endl, exit(0);
        else
            std::cout << "Server: " << buff << std::endl;
        
        if (count)
            write(write_fp[1], "HELLO FROM CLIENT", MAX_MSG), count--;
    }
}

int main(void) {
    int fpp[2], fpc[2];
    
    if (pipe(fpp) < 0)
        // ERROR Creating parent-child pipe
        return 1;
    
    if (fcntl(fpp[0], F_SETFL, O_NONBLOCK))
        // ERROR On non-blocking setup parent-child
        return 2;

    if (pipe(fpc) < 0)
        // ERROR Creating child-parent pipe
        return 4;
    
    if (fcntl(fpc[0], F_SETFL, O_NONBLOCK))
        // ERROR On non-blocking setup child-parent
        return 5;


    int pid = fork();

    if (pid < 0)
        // ERROR Forking
        return 3;

    else if (pid > 0) 
        // PARENT PROCESS
        parent(fpc, fpp);
    else 
        // CHILD PROCESS
        child(fpp, fpc);

    return 0;
}