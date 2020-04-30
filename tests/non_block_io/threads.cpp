#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>

#define MAX_MSG 4096

void parent(std::string& r_p, std::string& w_p, std::mutex& lock) {
    int count = 27;

    while (true) {
        lock.lock();
        if (r_p.length())
            std::cout << "Client sent: " << r_p << std::endl;
        else 
            std::cout << "(empty)" << std::endl;
        r_p.clear();
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        
        if (count) {
            while(w_p.length()) std::cout << "(busy)" << std::endl, std::this_thread::sleep_for(std::chrono::microseconds(50));

            w_p = "Hello from SERVER\n";
            count--;
        } 
    }
}

void child(std::string& r_p, std::string& w_p, std::mutex& lock) {
    int count = 10;

    while (true) {
        lock.lock();
        if (r_p.length()) 
            std::cout << "Server sent: " << r_p << std::endl;
        else 
            std::cout << "(empty)" << std::endl;
        r_p.clear();
        lock.unlock();

        std::this_thread::sleep_for(std::chrono::microseconds(1200));
        
        if (count) {
            while(w_p.length()) std::cout << "(busy)" << std::endl, std::this_thread::sleep_for(std::chrono::microseconds(50));
            
            w_p = "Hello from CLIENT\n";
            count--;
        }
    }
}

int main(void) {
    std::string p2c;
    std::string c2p;
    std::mutex lock;

    std::thread parent_t(parent, std::ref(c2p), std::ref(p2c), std::ref(lock));
    std::thread child_t(child, std::ref(p2c), std::ref(c2p), std::ref(lock));

    parent_t.join();
    child_t.join();

    return 0;
}