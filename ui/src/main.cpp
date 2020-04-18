#include <gtkmm.h>
#include <iostream>
#include <string>

auto app = Gtk::Application::create();
auto builder = Gtk::Builder::create_from_file("./glade/window2.glade");
Gtk::Window* window;
Gtk::Button* button;
Gtk::Label* hlabel;
Gtk::TextView* tview;
Gtk::ScrolledWindow* scrolled;
Gtk::Viewport* viewport;
Glib::RefPtr<Gtk::Adjustment> adj;
auto tbuffer = Gtk::TextBuffer::create();

void on_helloButton_clicked() {
	hlabel->set_label(hlabel->get_label() + '\n' + tbuffer->get_text());
	tbuffer->set_text("");
}

void on_scrolled_size_allocate(Gtk::Allocation& alocator) {
	adj->set_value(adj->get_upper() - adj->get_page_size());
}

int main() {

	builder->get_widget("window", window);
	builder->get_widget("helloButton", button);
	builder->get_widget("helloLabel", hlabel);
	builder->get_widget("textView", tview);
	builder->get_widget("scrolled", scrolled);
	builder->get_widget("viewport", viewport);

	scrolled->set_placement(Gtk::CornerType::CORNER_BOTTOM_LEFT);

	hlabel->signal_size_allocate().connect_notify(sigc::ptr_fun(&on_scrolled_size_allocate));

	adj = scrolled->get_vadjustment();

	tview->set_buffer(tbuffer);
	tview->set_wrap_mode(Gtk::WrapMode::WRAP_WORD_CHAR);



	button->signal_clicked().connect(sigc::ptr_fun(&on_helloButton_clicked));

	return app->run(*window);
}
