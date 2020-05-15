#include <gtkmm.h>
#include <string>
#include <map>

#ifndef CLIENT_H
#define CLIENT_H

typedef void (* Hook)(class Client *, std::string&);

class Client {
    private:
        Gtk::Window* chat_window;
        Gtk::Button* send_button;
        Gtk::Label* chat_label;
        Gtk::TextView* text_input;
        Gtk::ScrolledWindow* chat_scroll;
        Glib::RefPtr<Gtk::TextBuffer> input_buffer;
        Glib::RefPtr<Gtk::Adjustment> scroll_adjustment;

        std::map<std::string, Hook> executors;
        std::map<std::string, Hook> handlers;
        

        int hub_socket;

        void sender(std::string& str);
        void parse_command(std::string& str);

        // signal handlers
        void send_button_click();
        void auto_scroll(Gtk::Allocation& alocator);
        bool receiver();

    public:
        std::string nickname;
        bool connected;

        Client();
        
        Gtk::Window& get_window();
        void add_text(std::string text);
        void quit();
        void create_connection();

        ~Client();
};

#endif