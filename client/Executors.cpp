#include "Executors.h"
#include <iostream>

void Executors::connect_executor(Client* client, std::string& text) {
    if (client->connected) {
        client->add_text("ERROR: You are already connected.");
        return;
    }

    if (client->nickname.empty()) {
        client->add_text("ERROR: You need to choose a nickname first.\n\tUse: /nick <nickname>");
        return;
    }

    client->create_connection();
}

void Executors::ping_executor(Client* client, std::string& text) {
    std::cout << "ping executor" << std::endl;
}

void Executors::nick_executor(Client* client, std::string& text) {
    if (text.size() == 0) {
        client->add_text("ERROR: Usage is /nick <nickname>");
        return;
    }

    size_t arg1 = text.find(" ");
    std::string new_nick = text.substr(0, arg1);

    if (new_nick.size() > 20) {
        client->add_text("ERROR: Nickname must have less than 20 chars");
        return;
    }

    if (!client->connected) 
        client->nickname = new_nick;
    else
        // make request to change nickname 
        // client->change_nick(new_nick);
        client->nickname = new_nick;


    client->add_text("Nickname is set to " + client->nickname);
}

void Executors::quit_executor(Client* client, std::string& text) {

    client->quit();
}

void Executors::say_executor(Client* client, std::string& text) {
    std::cout << "say: " << text << std::endl;
}
