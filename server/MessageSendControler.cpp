#include "MessageSendController.h"

#include <iostream>

void MessageSendController::deduct() {
    if(clients == 1) std::cout << "message destructed\n";
    if(!--clients) delete this;
}

const char* MessageSendController::getBuffer() {
    return buffer.c_str();
}

void MessageSendController::setBuffer(std::string b) {
    buffer.assign(b.c_str());
}

MessageSendController::MessageSendController(int c) : clients(c) { 
} 

MessageSendController::~MessageSendController() {
    
}