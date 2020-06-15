#ifdef VS_HACK
    // This is not included on compilation, just in VS Code
    // to make IntelliSense work
    #include "PCHClient.h"
#endif

#include "Handlers.h"
#include "ui/Screen.h"

void Handlers::nickname_in_use_handler(Client* client, Message* msg) {
    Screen::log_message("Nickname <b>" + msg->params.getTrailing() + "</b> is already in use.", Screen::LogType::ERROR);
}

void Handlers::nickname_change_handler(Client* client, Message* msg) {
    client->nickname = msg->params.getTrailing();

    Screen::log_message("Nickname changed to " + client->nickname + ".", Screen::LogType::SUCCESS);
}

void Handlers::message_handler(Client* client, Message* msg) {
    std::string nick = msg->prefix.getNick();
    std::string trail = msg->params.getTrailing();
        
    Screen::add_message(std::ref(trail), std::ref(nick));

    // ACK MSG Received
    Message* response = new Message();
    response->command.set_cmd("ACK");
    response->prefix.setNick(client->nickname);

    client->send_message(response);
    
    delete response;
}

void Handlers::pong_handler(Client* client, Message* msg) {
    std::chrono::steady_clock::time_point begin = client->sentTime;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    Screen::log_message(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) + "ms until PONG", Screen::LogType::SUCCESS);
}


void Handlers::channel_join_handler(Client* client, Message* msg) {
    client->channel = msg->params.getTrailing();

    Screen::log_message("Joined channel " + client->channel + ".", Screen::LogType::SUCCESS);
}


void Handlers::channel_op_privs_needed_handler(Client* client, Message* msg) {
    Screen::log_message("You need to be channel OP to do this!", Screen::LogType::ERROR);
}

void Handlers::kicked_from_channel_handler(Client* client, Message* msg) {
    Screen::log_message("You have been kicked from #" + client->channel + ".", Screen::LogType::SUCCESS);

    client->channel = "";
}

void Handlers::who_is_response_handler(Client* client, Message* msg) {
    Screen::log_message("O usuário " + msg->params.getMiddleContent()[0] + " possui o IP " + msg->params.getTrailing(), Screen::LogType::SUCCESS);
}

void Handlers::mute_handler(Client* client, Message* msg) {
    Screen::log_message("O usuário " + msg->params.getMiddleContent()[0] + " foi mutado", Screen::LogType::SUCCESS);
}

void Handlers::mute_warning_handler(Client* client, Message* msg) {
    Screen::log_message("O admin do canal te mutou", Screen::LogType::SUCCESS);
}

void Handlers::unmute_handler(Client* client, Message* msg) {
    Screen::log_message("O usuário " + msg->params.getMiddleContent()[0] + " foi desmutado", Screen::LogType::SUCCESS);
}

void Handlers::unmute_warning_handler(Client* client, Message* msg) {
    Screen::log_message("O admin do canal te desmutou", Screen::LogType::SUCCESS);
}