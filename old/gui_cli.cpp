#include <gtkmm.h>
#include <iostream>
#include <string>
#include <algorithm>

#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "../utils/RFCprotocol.h"


#define SERVER_DOOR 9030
#define LOG 1

void sender(std::string& send, int hub_socket, std::string& nick);
bool receiver(Gtk::Label* chat_window, int hub_socket);

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

// pointer to int that stores socket number (to close with sign. handl.)
static volatile int *socket_ptr = NULL;

// Signal Handler to kill socket on Ctrl+C (SIGINT)
void shutDown(int dummy) {
		
    shutdown(*socket_ptr, SHUT_RDWR);
    close(*socket_ptr);

    if (LOG) std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;

    exit(0);
}

// "OnClick" function, executed every time the "Send" button is clicked
void send_button_click(Gtk::TextBuffer* input_buffer, int hub_socket, std::string& nick) {
	// send message to HUB
	std::string send = input_buffer->get_text();

	send = trim(send);
	sender(std::ref(send), hub_socket, nick);

	input_buffer->set_text("");
}

// Called every time the label changes, to make sure that the scroll is always at bottom
void set_scroll_label(Gtk::Allocation& alocator, Gtk::Adjustment* scroll_adjustment) {
	scroll_adjustment->set_value(
		scroll_adjustment->get_upper() - scroll_adjustment->get_page_size());
}

// Function to send message to HUB
// 	send_str - message to be sent
// 	hub_socket - socket number (file) -- where to send the message
// 	nick - nickname of the user
void sender(std::string& send_str, int hub_socket, std::string& nick) {

	if (send_str.size()) {
		// if there is msg, send_str it to the socket (server)
		std::replace(send_str.begin(), send_str.end(), '\n', ' ');
		std::replace(send_str.begin(), send_str.end(), '\r', ' ');

		// break the message each 4000 chars (not 4096), to accommodate the headers of protocol
		int msg_num = send_str.size() / 4000;
		if (send_str.size() % 4000) 
			msg_num++;

		if (LOG) std::cout << "CLIENT_LOG: num of messeges = " << msg_num << std::endl;

		for (int i = 0; i < msg_num; i++){

			Messege msg = Messege();

			msg.prefix.setNick(nick);
			msg.command.setWord("/say");
			msg.params.setTrailing(send_str.substr(i * 4000, 4000));
			
			char serld_msg[4096];
			
			strncpy(serld_msg, msg.serializeMessege().c_str(), 4096);

			if (LOG) std::cout << "CLIENT_LOG: Msg" << i + 1 << ">" << serld_msg << std::endl;

			send(hub_socket, serld_msg, sizeof(serld_msg), 0);
		}

		send_str.clear();
	}
}

// Function to reveive a message from the HUB (timeout handler, called each 100 milliseconds)
// 	chat_window - where to add the message
// 	hub_socket - socket number -- where to read from
bool receiver(Gtk::Label* chat_window, int hub_socket) {

	while (true) {
		// if receive data from socket, write in screen
		char msg[4096];
		int res = recv(hub_socket, &msg, 4096, 0);

		if (res > 0) {
			if (LOG) std::cout << "CLIENT_LOG: Message>" << msg << std::endl;

			Messege msgObj = Messege(msg);
			std::string msg_str = msgObj.prefix.getNick() + ": " + msgObj.params.getTrailing();

			chat_window->set_label(chat_window->get_label() + '\n' + msg_str);

		} else {
			if (res == 0){
				if (LOG) std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;
				exit(2);
			}
			if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
				if (LOG) std::cout << "CLIENT_LOG: Error receiving messege from hub: " << errno << std::endl; 

			break;
		}
	}

	return true;
}

int main() {
    signal(SIGINT, shutDown);

	// BEGIN GUI

	auto app = Gtk::Application::create();
	auto builder = Gtk::Builder::create_from_file("./ui/window.glade");
	Gtk::Window* chat_window;
	Gtk::Button* send_button;
	Gtk::Label* chat_label;
	Gtk::TextView* text_input;
	Gtk::ScrolledWindow* chat_scroll;
	Glib::RefPtr<Gtk::TextBuffer> input_buffer = Gtk::TextBuffer::create();

	builder->get_widget("chat_window", chat_window);
	builder->get_widget("send_button", send_button);
	builder->get_widget("chat_label", chat_label);
	builder->get_widget("text_input", text_input);
	builder->get_widget("chat_scroll", chat_scroll);

	auto scroll_adjustment = chat_scroll->get_vadjustment();

	chat_scroll->set_placement(Gtk::CornerType::CORNER_BOTTOM_LEFT);
	
	text_input->set_buffer(input_buffer);
	text_input->set_wrap_mode(Gtk::WrapMode::WRAP_WORD_CHAR);

	chat_label->set_line_wrap(true);
	chat_label->set_line_wrap_mode(Pango::WrapMode::WRAP_WORD_CHAR);
	chat_label->set_justify(Gtk::JUSTIFY_LEFT);

	// END GUI



	// BEGIN SOCKET

	// Socket opening
	int hub_socket = socket(AF_INET, SOCK_STREAM, 0);
	socket_ptr = &hub_socket;


	struct  sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(hub_socket, (struct sockaddr*) &hub_address, sizeof(hub_address));

	if (LOG) 
		if (connection_status)
			if (LOG) std::cout << "CLIENT_LOG: Failed to connect to hub" << std::endl;
		else 
			if (LOG) std::cout << "CLIENT_LOG: Connected to hub" << std::endl;
	
	// If there is connection error, shutdown
	if (connection_status) return 1;

	// Non-blocking I/O flag is set
    fcntl(hub_socket, F_SETFL, O_NONBLOCK);

	// END SOCKET



	// BEGIN NICK

	std::string nick;

	do {
		std::cout << "Insira um nick de até 20 caracteres" << std::endl;
		std::cin >> nick;
	} while (nick.size() > 20);

	// END NICK



	// BEGIN SIGNALS
	// Setup all signals from GUI, to control read/write functions

	// On button click, send message to HUB
	send_button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&send_button_click), input_buffer.get(), hub_socket, std::ref(nick)));

	// Scroll to bottom on value change in label
	chat_label->signal_size_allocate().connect_notify(sigc::bind(sigc::ptr_fun(&set_scroll_label), (scroll_adjustment.get())));

	// Timeout to read attempt
	Glib::signal_timeout().connect(sigc::bind<Gtk::Label*, int>(sigc::ptr_fun(&receiver), chat_label, hub_socket), 100);

	// END SIGNALS



	// RUN
	app->run(*chat_window);



	//SHUTDOWN
    shutdown(hub_socket, SHUT_RDWR);
	close(hub_socket);

	return 0;
}
