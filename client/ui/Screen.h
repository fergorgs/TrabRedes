#include <gtkmm.h>
#include "../Client.h"

#ifndef SCREEN_H
#define SCREEN_H

namespace Screen {
    enum LogType {
        ERROR,
        WARNING,
        SUCCESS
    };

    extern Gtk::Window* window;

    void setup(Client* cli);
    void add_message(std::string& msg, std::string& nick);
    void log_message(std::string msg, LogType type);

}

#endif