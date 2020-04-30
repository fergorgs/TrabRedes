#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <string>

#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_DOOR 9030
#define LOG 1


static volatile int *hubSocketAddr = NULL;

void shutDown(int dummy){
    
    std::cout << std::endl << "DISCONNECTED\n";
	
    shutdown(*hubSocketAddr, SHUT_RDWR);
    close(*hubSocketAddr);

    exit(0);

}

void on_helloButton_clicked(std::string& msg, Gtk::TextBuffer* buff) {
	if (msg.size()) 
		return;
	msg = buff->get_text();
	buff->set_text("");
}

void on_scrolled_size_allocate(Gtk::Allocation& alocator, Gtk::Adjustment* adj) {
	adj->set_value(adj->get_upper() - adj->get_page_size());
}

void sender(std::string& s, int hub) {
	while(true) {
		if (s.size()) {
			// if there is msg, send it to the socket (server)
			int i = 0;
			while(true) {
				std::cout << i;
				send(hub, s.c_str() + i, 4096, 0);
				if (LOG) std::cout << "CLIENT_LOG: Sent to HUB> " << (s.c_str() + i) << std::endl;
				if(s.size() - i > 4096) i += 4096;
				else {
					s.clear();
					break;
				}
			} 
		}
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void receiver(Gtk::Label* chat_window, int hub) {
	while(true) {
		// if receive data from socket, write in screen
		char msg[4096];
		int res = recv(hub, &msg, 4096, 0);
		switch (res) {
			case 0:
				if (LOG) 
					std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;
					// kill client
				break;
			
			case -1:
				if(errno != EAGAIN && errno != EWOULDBLOCK && LOG)
					std::cout << "CLIENT_LOG: Error receiving messege from hub: " << errno << std::endl; 
				break;

			default:
				std::string msg_str(msg);
				chat_window->set_label(chat_window->get_label() + '\n' + msg_str);
				std::cout << "CLIENT_LOG: Received message>" << msg_str << std::endl;
		} 

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

int main() {
	//TODO: Modularizar codigo

    signal(SIGINT, shutDown);

	// BEGIN GUI
	auto app = Gtk::Application::create();
	auto builder = Gtk::Builder::create_from_file("./ui/window.glade");
	Gtk::Window* window;
	Gtk::Button* button;
	Gtk::Label* hlabel;
	Gtk::TextView* tview;
	Gtk::ScrolledWindow* scrolled;
	Gtk::Viewport* viewport;
	auto tbuffer = Gtk::TextBuffer::create();

	builder->get_widget("window", window);
	builder->get_widget("helloButton", button);
	builder->get_widget("helloLabel", hlabel);
	builder->get_widget("textView", tview);
	builder->get_widget("scrolled", scrolled);
	builder->get_widget("viewport", viewport);

	auto adj = scrolled->get_vadjustment();

	scrolled->set_placement(Gtk::CornerType::CORNER_BOTTOM_LEFT);

	hlabel->signal_size_allocate().connect_notify(sigc::bind(sigc::ptr_fun(&on_scrolled_size_allocate), (adj.get())));

	tview->set_buffer(tbuffer);
	tview->set_wrap_mode(Gtk::WrapMode::WRAP_WORD_CHAR);

	std::string sent_msg = "";

	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&on_helloButton_clicked), std::ref(sent_msg), (tbuffer.get())));
	// END GUI



	// BEGIN SOCKET
	// abertura do socket
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
	
	// se tiver erro na conexao fecha o programa
	if (connection_status) return 0;

	// seta flag para I/O nao bloqueante
    fcntl(hub_socket, F_SETFL, O_NONBLOCK);
	// END SOCKET



	// threads para comunicar socket com GUI
	std::thread sender_t(sender, std::ref(sent_msg), hub_socket); // mandar como arg
	std::thread receiver_t(receiver, hlabel, hub_socket); // mandar como arg
	
	app->run(*window);

	sender_t.join();
	receiver_t.join();


    shutdown(hub_socket, SHUT_RDWR);
	close(hub_socket);

	return 1337;
}
