#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstdlib>

#define MAX_MSG 4096

void parent(std::string& r_p, std::string& w_p, std::mutex& lock) {
    system("bash -e './child'");
}

void child(std::string& r_p, std::string& w_p, std::mutex& lock) {
    system("./child");
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