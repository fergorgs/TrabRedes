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

    Screen::log_message("Nickname changed to " + msg->params.getTrailing() + ".", Screen::LogType::SUCCESS);
}

void Handlers::message_handler(Client* client, Message* msg) {
    std::string nick = msg->prefix.getNick();
    std::string trail = msg->params.getTrailing();
        
    Screen::add_message(std::ref(trail), std::ref(nick));

}

void Handlers::pong_handler(Client* client, Message* msg) {


    //std::cout << "Pong handler" << endl;

    std::chrono::steady_clock::time_point begin = client->sentTime;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    //Screen::log_message("log message test", Screen::LogType::SUCCESS);
    Screen::log_message(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()), Screen::LogType::SUCCESS);

}