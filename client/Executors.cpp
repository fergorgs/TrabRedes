#include "Executors.h"
#include "Client.h"
#include "../utils/RFCprotocol.h"

#include <vector>
#include <iostream>

#define MSG_TRUNC 4000

void Executors::connect_executor(Client* client, std::string& text) {
    if (client->connected) {
        client->add_text("ERROR: You are already connected.");
        return;
    }

    client->create_connection();
    client->add_text("You are connected.");
}

void Executors::ping_executor(Client* client, std::string& text) {
    std::cout << "ping executor" << std::endl;
}

void Executors::nick_executor(Client* client, std::string& text) {
    if (!client->connected) {
        client->add_text("ERROR: Connect to server to change your nick (/connect).");
        return;
    }

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

    // make request to change nickname 
    // client->change_nick(new_nick);
    client->nickname = new_nick;

    client->add_text("Nickname is set to " + client->nickname);
}

void Executors::quit_executor(Client* client, std::string& text) {
    client->quit();
}

void Executors::say_executor(Client* client, std::string& text) {
    for (int i = 0; i * MSG_TRUNC < text.size(); i++) {
        Message* msg = new Message();

        msg->prefix.setNick(client->nickname);
        msg->command.setWord("say");
        msg->params.setTrailing(text.substr(i * MSG_TRUNC, MSG_TRUNC));

        client->send_message(msg);

        delete msg;
    }    
}
