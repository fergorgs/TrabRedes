#include "Executors.h"

#include "../utils/RFCprotocol.h"
#include "ui/Screen.h"

#define MSG_MAX_SIZE 4000

void Executors::connect_executor(Client* client, std::string& text) {
    if (client->connected) {
        Screen::log_message("You are already connected.", Screen::LogType::ERROR);
        return;
    }

    client->create_connection();
}

void Executors::ping_executor(Client* client, std::string& text) {
    std::cout << "ping executor" << std::endl;
}

void Executors::nick_executor(Client* client, std::string& text) {
    if (!client->connected) {
        Screen::log_message("Connect to server to change your nick (/connect).", Screen::LogType::ERROR);
        return;
    }

    if (text.size() == 0) {
        Screen::log_message("Usage is /nick <nickname>", Screen::LogType::ERROR);
        return;
    }

    size_t arg1 = text.find(" ");
    std::string new_nick = text.substr(0, arg1);

    if (new_nick.size() > 20) {
        Screen::log_message("Nickname must have less than 20 chars.", Screen::LogType::ERROR);
        return;
    }

    // make request to change nickname 
    // client->change_nick(new_nick);
    client->nickname = new_nick;

    Screen::log_message("Nickname is set to " + client->nickname, Screen::LogType::SUCCESS);
}

void Executors::quit_executor(Client* client, std::string& text) {
    client->quit();
}

void Executors::say_executor(Client* client, std::string& text) {
    if (!client->connected) {
        Screen::log_message("You need to be connected to do this.", Screen::LogType::ERROR);
        return;
    }

    for (int i = 0; i * MSG_MAX_SIZE < text.size(); i++) {
        Message* msg = new Message();

        msg->prefix.setNick(client->nickname);
        msg->command.setWord("say");
        msg->params.setTrailing(text.substr(i * MSG_MAX_SIZE, MSG_MAX_SIZE));

        client->send_message(msg);

        delete msg;
    }    
}
