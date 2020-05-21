#include "Screen.h"
#include <gtkmm.h>

int main(void) {

    auto app = Gtk::Application::create();
    Screen::setup();

    app->run(*Screen::window);

    return 0;
}