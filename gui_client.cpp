#include <gtkmm.h>
#include <iostream>
#include <string>
#include <stdio.h>

#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "RFCprotocol.h"

using namespace std;

#define SERVER_DOOR 9030
#define LOG 1

void sender(std::string& s, int hub_socket);
bool receiver(Gtk::Label* chat_window, int hub_socket);

static volatile int *hubSocketAddr = NULL;

void shutDown(int dummy) {
    
    std::cout << std::endl << "DISCONNECTED\n";
	
    shutdown(*hubSocketAddr, SHUT_RDWR);
    close(*hubSocketAddr);

    exit(0);

}

void send_button_click(Gtk::TextBuffer* input_buffer, int hub_socket) {
	// send message to HUB
	std::string message = input_buffer->get_text();
	sender(std::ref(message), hub_socket);

	input_buffer->set_text("");
}

void set_scroll_label(Gtk::Allocation& alocator, Gtk::Adjustment* scroll_adjustment) {
	scroll_adjustment->set_value(
		scroll_adjustment->get_upper() - scroll_adjustment->get_page_size());
}

void sender(std::string& s, int hub) {

	if (s.size()) {
		// if there is msg, send it to the socket (server)
		//int i = 0;
		int numOfMessages = s.size()/4000;
		if(s.size()%4000) numOfMessages++;

		if(LOG) cout << "CLIENT_LOG: num of messeges = " << numOfMessages << endl;

		for(int i = 0; i < numOfMessages; i++){

			Messege msg = Messege();

			msg.prefix.setNick("");
			msg.command.setWord("/say");
			msg.params.setTrailing(s.substr(i * 4000, 4000));
			
			char serldMsg[4096];
			
			strcpy(serldMsg, msg.serializeMessege().c_str());

			if (LOG) std::cout << "CLIENT_LOG: Msg" << i + 1 << ">" << serldMsg << std::endl;

			send(hub, serldMsg, sizeof(serldMsg), 0);
		}
		s.clear();
	}
}


bool receiver(Gtk::Label* chat_window, int hub_socket) {
	while(true) {
		// if receive data from socket, write in screen
		char msg[4096];
		int res = recv(hub_socket, &msg, 4096, 0);

		if (res > 0) {
			if (LOG) std::cout << "CLIENT_LOG: Message>" << msg << std::endl;

			Messege msgStr = Messege(msg);
            char deSerldMsg[4096];
            strcpy(deSerldMsg, msgStr.params.getTrailing().c_str());

			std::string msg_str(deSerldMsg);
			chat_window->set_label(chat_window->get_label() + '\n' + msg_str);

			if (LOG) std::cout << "CLIENT_LOG: DeSerMsg>" << msg_str << std::endl;
		} else {
			if (res == 0)
				std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;
			if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK && LOG)
				std::cout << "CLIENT_LOG: Error receiving messege from hub: " << errno << std::endl; 

			break;
		}
	}
	

	return true;
}

int main() {
	//TODO: Modularizar codigo

    signal(SIGINT, shutDown);

	// BEGIN GUI

	auto app = Gtk::Application::create();
	auto builder = Gtk::Builder::create_from_file("./ui/window.glade");
	Gtk::Window* chat_window;
	Gtk::Button* send_button;
	Gtk::Label* chat_label;
	Gtk::TextView* text_input;
	Gtk::ScrolledWindow* chat_scroll;
	auto input_buffer = Gtk::TextBuffer::create();

	builder->get_widget("window", chat_window);
	builder->get_widget("helloButton", send_button);
	builder->get_widget("helloLabel", chat_label);
	builder->get_widget("textView", text_input);
	builder->get_widget("scrolled", chat_scroll);

	auto scroll_adjustment = chat_scroll->get_vadjustment();

	chat_scroll->set_placement(Gtk::CornerType::CORNER_BOTTOM_LEFT);
	
	text_input->set_buffer(input_buffer);
	text_input->set_wrap_mode(Gtk::WrapMode::WRAP_WORD_CHAR);

	chat_label->set_line_wrap(true);
	chat_label->set_line_wrap_mode(Pango::WrapMode::WRAP_WORD_CHAR);

	// END GUI



	// BEGIN SOCKET

	// Socket opening
	int hub_socket = socket(AF_INET, SOCK_STREAM, 0);
	hubSocketAddr = &hub_socket;


	struct  sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(hub_socket, (struct sockaddr*) &hub_address, sizeof(hub_address));

	if (LOG) 
		if (connection_status)
			std::cout << "CLIENT_LOG: Failed to connect to hub" << std::endl;
		else 
			std::cout << "CLIENT_LOG: Connected to hub" << std::endl;
	
	// If there is connection error, shutdown
	if (connection_status) return -1;

	// Non-blocking I/O flag is set
    fcntl(hub_socket, F_SETFL, O_NONBLOCK);

	// END SOCKET



	// BEGIN SIGNALS
	// Setup all signals from GUI, to control read/write functions

	// On button click, send message to HUB
	send_button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&send_button_click), input_buffer.get(), hub_socket));

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
