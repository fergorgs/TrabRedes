#include <gtkmm.h>
#include <string>

#ifndef CLIENT_H
#define CLIENT_H

class Client {
    private:
        Gtk::Window* chat_window;
        Gtk::Button* send_button;
        Gtk::Label* chat_label;
        Gtk::TextView* text_input;
        Gtk::ScrolledWindow* chat_scroll;
        Glib::RefPtr<Gtk::TextBuffer> input_buffer;
        Glib::RefPtr<Gtk::Adjustment> scroll_adjustment;

        std::string nickname;
        int hub_socket;

        bool connected;

        void create_connection();
        void sender(std::string& str);
        void parse_command(std::string& str);
        void quit();

        // signal handlers
        void send_button_handler();
        void auto_scroll(Gtk::Allocation& alocator);
        bool receiver();
    public:
        Client();
        
        Gtk::Window& get_window();

        ~Client();
};

#endif