#include "Client.h"
#include <gtkmm.h>
#include <iostream>
#include <string>
#include <algorithm>

#include <netinet/in.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "../utils/RFCprotocol.h"

#define SERVER_DOOR 9030
#define LOG 1


// STR MANIP
// BEGIN
std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);

// Trim start of string
std::string ltrim(const std::string& s) {
	const std::string WHITESPACE = " \n\r\t\f\v";
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

// Trim end of string
std::string rtrim(const std::string& s) {
	const std::string WHITESPACE = " \n\r\t\f\v";
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

// Trim string
std::string trim(const std::string& s) {
	return rtrim(ltrim(s));
}
// END

// Private functions
void Client::create_connection() {
    
    // Socket opening
    hub_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(hub_socket, (struct sockaddr*) &hub_address, sizeof(hub_address));

    if (connection_status)
        if (LOG) std::cout << "CLIENT_LOG: Failed to connect to hub" << std::endl;
    else 
        if (LOG) std::cout << "CLIENT_LOG: Connected to hub" << std::endl;
	
	// If there is connection error, shutdown
	if (connection_status) exit(1);

	// Non-blocking I/O flag is set
    fcntl(hub_socket, F_SETFL, O_NONBLOCK);
}


void Client::send_button_handler() {
    std::string str_msg = input_buffer->get_text();

	str_msg = trim(str_msg);

    if (str_msg.size()) {
		// if there is msg, str_msg it to the socket (server)
		std::replace(str_msg.begin(), str_msg.end(), '\n', ' ');
		std::replace(str_msg.begin(), str_msg.end(), '\r', ' ');

		// break the message each 4000 chars (not 4096), to accommodate the headers of protocol
		int msg_num = str_msg.size() / 4000;
		if (str_msg.size() % 4000) 
			msg_num++;

		if (LOG) std::cout << "CLIENT_LOG: num of messeges = " << msg_num << std::endl;

		for (int i = 0; i < msg_num; i++){

			Messege msg = Messege();

			msg.prefix.setNick(nickname);
			msg.command.setWord("/say");
			msg.params.setTrailing(str_msg.substr(i * 4000, 4000));
			
			char serld_msg[4096];
			
			strncpy(serld_msg, msg.serializeMessege().c_str(), 4096);

			if (LOG) std::cout << "CLIENT_LOG: Msg" << i + 1 << ">" << serld_msg << std::endl;

			send(hub_socket, serld_msg, sizeof(serld_msg), 0);
		}

		str_msg.clear();
	}

	input_buffer->set_text("");
}

void Client::auto_scroll(Gtk::Allocation& alocator) {
    scroll_adjustment->set_value(
		scroll_adjustment->get_upper() - scroll_adjustment->get_page_size());
}

// Function to reveive a message from the HUB (timeout handler, called each 100 milliseconds)
bool Client::receiver() {
    while (true) {
		// if receive data from socket, write in screen
		char msg[4096];
		int res = recv(hub_socket, &msg, 4096, 0);

		if (res > 0) {
			if (LOG) std::cout << "CLIENT_LOG: Message>" << msg << std::endl;

			Messege msgObj = Messege(msg);
			std::string msg_str = msgObj.prefix.getNick() + ": " + msgObj.params.getTrailing();

			chat_label->set_label(chat_label->get_label() + '\n' + msg_str);

		} else {
			if (res == 0){
				if (LOG) std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;
                if (LOG) std::cout << "CLIENT_LOG: Shutting Client down" << std::endl;

				exit(2);
			}
			if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
				if (LOG) std::cout << "CLIENT_LOG: Error receiving messege from hub: " << errno << std::endl; 

			break;
		}
	}

	return true;
}

// Public functions
Client::Client() {
    auto builder = Gtk::Builder::create_from_file("./client/ui/window.glade");

	builder->get_widget("chat_window", chat_window);
	builder->get_widget("send_button", send_button);
	builder->get_widget("chat_label", chat_label);
	builder->get_widget("text_input", text_input);
	builder->get_widget("chat_scroll", chat_scroll);

    input_buffer = Gtk::TextBuffer::create();
    scroll_adjustment = chat_scroll->get_vadjustment();

	chat_scroll->set_placement(Gtk::CornerType::CORNER_BOTTOM_LEFT);
	
	text_input->set_buffer(input_buffer);
	text_input->set_wrap_mode(Gtk::WrapMode::WRAP_WORD_CHAR);

	chat_label->set_line_wrap(true);
	chat_label->set_line_wrap_mode(Pango::WrapMode::WRAP_WORD_CHAR);
	chat_label->set_justify(Gtk::JUSTIFY_LEFT);

	// On button click, send message to HUB
	send_button->signal_clicked().connect(sigc::mem_fun(*this, &Client::send_button_handler));

	// Scroll to bottom on value change in label
	chat_label->signal_size_allocate().connect_notify(sigc::mem_fun(*this, &Client::auto_scroll));

	// Timeout to read attempt
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &Client::receiver), 100);

    nickname = "dummy";

    create_connection();
}

Gtk::Window& Client::get_window() {
    return *chat_window;
}

Client::~Client() {
    shutdown(hub_socket, SHUT_RDWR);
	close(hub_socket);

    if (LOG) std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;
    if (LOG) std::cout << "CLIENT_LOG: Shutting Client down" << std::endl;
}