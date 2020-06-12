#ifdef VS_HACK
    // This is not included on compilation, just in VS Code
    // to make IntelliSense work
    #include "PCHClient.h"
#endif

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

    Screen::log_message("Use /join [channel] to join a channel.", Screen::LogType::SUCCESS);
}

void Executors::ping_executor(Client* client, std::string& text) {
    if (!client->connected) {
        Screen::log_message("You need to be connected to do this ( /connect ).", Screen::LogType::ERROR);
        return;
    }

    Message* msg = new Message();

    msg->prefix.setNick(client->nickname);
    msg->command.set_cmd("PING");
    msg->params.setTrailing("Hello from earth");

    client->send_message(msg);

    client->sentTime = std::chrono::steady_clock::now();

    delete msg;
}

void Executors::nick_executor(Client* client, std::string& text) {
    if (!client->connected) {
        Screen::log_message("Connect to server to change your nick ( /connect ).", Screen::LogType::ERROR);
        return;
    }

    if (text.size() == 0) {
        Screen::log_message("Usage is /nick [nickname]", Screen::LogType::ERROR);
        return;
    }

    size_t arg1 = text.find(" ");
    std::string new_nick = text.substr(0, arg1);

    if (new_nick.size() > 20) {
        Screen::log_message("Nickname must have less than 20 chars.", Screen::LogType::ERROR);
        return;
    }

    Message* msg = new Message();
    msg->command.set_cmd("NICK");
    msg->params.setTrailing(new_nick);

    client->send_message(msg);

    delete msg;
}

void Executors::quit_executor(Client* client, std::string& text) {
    client->quit();
}

void Executors::say_executor(Client* client, std::string& text) {
    if (!client->connected) {
        Screen::log_message("You need to be connected to do this.", Screen::LogType::ERROR);
        return;
    }

    if (client->nickname.empty()) {
        Screen::log_message("Select your nickname first ( /nick [nickname] ).", Screen::LogType::ERROR);
        return;
    }

    if (client->channel.empty()) {
        Screen::log_message("Join a Channel first ( /join [channel] ).", Screen::LogType::ERROR);
        return;
    }

    for (int i = 0; i * MSG_MAX_SIZE < text.size(); i++) {
        Message* msg = new Message();

        msg->prefix.setNick(client->nickname);
        msg->command.set_cmd("SAY");
        msg->params.setTrailing(text.substr(i * MSG_MAX_SIZE, MSG_MAX_SIZE));

        client->send_message(msg);

        delete msg;
    }    
}

void Executors::join_executor(Client* client, std::string& text) {
    if (!client->connected) {
        Screen::log_message("You need to be connected to do this.", Screen::LogType::ERROR);
        return;
    }

    Message* join = new Message();

    join->command.set_cmd("JOIN");
    join->params.setTrailing(text);

    client->send_message(join);

    delete join;
}

void Executors::kick_executor(Client* client, std::string& text) {
    if (!client->connected) {
        Screen::log_message("You need to be connected to do this.", Screen::LogType::ERROR);
        return;
    }

    if (client->channel.empty()) {
        Screen::log_message("Join a Channel first ( /join [channel] ).", Screen::LogType::ERROR);
        return;
    }

    std::size_t arg1 = text.find(" ");
    std::size_t size = text.size();
    std::string nick = text.substr(0, arg1);
    std::string reason = text.substr(arg1 + 1);

    Message* kick = new Message();

    kick->command.set_cmd("KICK");
    kick->params.addMiddleParam(nick);
    kick->params.setTrailing(reason);

    client->send_message(kick);

    delete kick;
}