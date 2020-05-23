#include "Message.h"

#include <iostream>

void Message::deduct() {
    if(clients == 1) std::cout << "message destructed\n";
    if(!--clients) delete this;

}

const char* Message::getBuffer() {
    return buffer.c_str();
}

void Message::setBuffer(char* b) {
    buffer.assign(b);
}

Message::Message(int c) : clients(c) { 
} 

Message::~Message() {
    
}