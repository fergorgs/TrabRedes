#include <gtkmm.h>
#include <iostream>
#include <signal.h>

#include "Client.h"

void cancel(int sig) {
    std::cout << "What are you trying to do? Don't do that! >:C" << std::endl;
}

int main() {
    signal(SIGINT, cancel);

    auto app = Gtk::Application::create();
    Client* client = new Client();

    app->run(client->get_window());

    delete client;
}