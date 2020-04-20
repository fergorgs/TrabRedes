#include <gtkmm.h>
#include <iostream>
#include <thread>
#include <string>


void on_helloButton_clicked(std::string& msg, Gtk::TextBuffer* buff) {
	msg = buff->get_text();
	buff->set_text("");
}

void on_scrolled_size_allocate(Gtk::Allocation& alocator, Gtk::Adjustment* adj) {
	adj->set_value(adj->get_upper() - adj->get_page_size());
}

void sender(std::string& s) {
	while(true) {
		if (s.size())
			// if there is msg, send it to the socket (server)
			std::cout << "msg: " << s << std::endl, s.clear();
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
}

void receiver(Gtk::Label* chat_window) {
	while(true) {
		// if receive data from socket, write in screen
		chat_window->set_label(chat_window->get_label() + '\n' + "aaaaaaa");
        std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main() {
	auto app = Gtk::Application::create();
	auto builder = Gtk::Builder::create_from_file("./glade/window2.glade");
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

	std::thread sender_t(sender, std::ref(sent_msg));
	std::thread receiver_t(receiver, hlabel);
	
	int r = app->run(*window);

	sender_t.join();

	return r;
}
