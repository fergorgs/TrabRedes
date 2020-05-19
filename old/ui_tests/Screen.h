#include <gtkmm.h>

#ifndef SCREEN_H
#define SCREEN_H

namespace Screen {
    extern Gtk::Window* window;

    void setup();
    void add_message(std::string& msg, std::string& nick);
}

#endif