#ifdef VS_HACK
    // This is not included on compilation, just in VS Code
    // to make IntelliSense work
    #include "PCHClient.h"
#endif

#include "Client.h"

#include "../utils/StrManip.h"
#include "Executors.h"
#include "Handlers.h"
#include "ui/Screen.h"

#define SERVER_DOOR 9030
#define LOG 1


// Private functions
void Client::create_connection() {
    // Socket opening
    hub_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in hub_address;
    hub_address.sin_family = AF_INET;
    hub_address.sin_port = htons(SERVER_DOOR);
    hub_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(hub_socket, (struct sockaddr*) &hub_address, sizeof(hub_address));
	
	// If there is connection error, shutdown
	if (connection_status) {
        Screen::log_message("Failed to connect (" + to_string(errno) + ").", Screen::LogType::ERROR);
        if (LOG) std::cout << "CLIENT_LOG: Failed to connect to hub" << std::endl;
        return;
    }

	// Non-blocking I/O flag is set
    fcntl(hub_socket, F_SETFL, O_NONBLOCK);

    connected = true;
    Screen::log_message("You are connected.", Screen::LogType::SUCCESS);
    if (LOG) std::cout << "CLIENT_LOG: Connected to hub" << std::endl;
}

void Client::send_message(Message* msg) {
	std::string serialized_msg = msg->serializeMessage();

	if (LOG) std::cout << "CLIENT_LOG: Sending >" << serialized_msg << std::endl;

	send(hub_socket, serialized_msg.c_str(), 4096, 0);
}

void Client::parse_command(std::string& str) {
    if (str[0] == '/') {
        size_t arg0 = str.find(" ");
        std::string cmd = trim(str.substr(1, arg0));
        for (char& c : cmd) c = tolower(c); // to lower case

        std::string args = "";
        if (arg0 != std::string::npos) 
            args = trim(str.substr(arg0 + 1));

        if (executors.count(cmd) > 0)
            executors[cmd](this, std::ref(args));
        else
            Screen::log_message("It seems like this command doesn't exist.", Screen::LogType::ERROR);

    } else {
        executors["say"](this, str);
    }
}

// Function to reveive a message from the HUB (timeout handler, called each 100 milliseconds)
bool Client::receiver() {
    if (!connected) return true;

    while (true) {
		// if receive data from socket, write in screen
		char c_msg[4096];
		int res = recv(hub_socket, &c_msg, 4096, 0);

		std::string msg(c_msg);

		if (res > 0) {
			if (LOG) std::cout << "CLIENT_LOG: Message (" << msg << ")" << std::endl;

			Message* msg_obj = new Message(std::ref(msg));

            std::string cmd_id = msg_obj->command.get_id();
            for (char& c : cmd_id) c = tolower(c); // to lower case

            if (handlers.count(cmd_id) > 0)
                handlers[cmd_id](this, msg_obj);
            else
                Screen::log_message("Couldn't handle message received from server.", Screen::LogType::ERROR);


            delete msg_obj;
		} else {
			if (res == 0){
				if (LOG) std::cout << "CLIENT_LOG: Disconnected from HUB (" << to_string(errno) << ")." << std::endl;
                
                Screen::log_message("Disconnected from server.", Screen::LogType::ERROR);
                connected = false;
			} else if (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK) 
				if (LOG) std::cout << "CLIENT_LOG: Error receiving message from hub (" << errno << ")" << std::endl; 

			break;
		}
	}
	return true;
}

void Client::quit() {
    if (connected) {
        shutdown(hub_socket, SHUT_RDWR);
        close(hub_socket);
    }

    if (LOG) std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;
    if (LOG) std::cout << "CLIENT_LOG: Shutting Client down" << std::endl;

    exit(0);
}

Client::Client() {
    channel = "";
    nickname = "";
    std::chrono::steady_clock::time_point sentTime;
    connected = false;

    // Setup Screen & Signals
    Screen::setup(this);

    // Setup executors
    executors["quit"] = Executors::quit_executor;
    executors["connect"] = Executors::connect_executor;
    executors["nick"] = Executors::nick_executor;
    executors["ping"] = Executors::ping_executor;
    executors["say"] = Executors::say_executor;
    executors["join"] = Executors::join_executor;

    handlers["433"] = Handlers::nickname_in_use_handler;
    handlers["nick"] = Handlers::nickname_change_handler;
    handlers["say"] = Handlers::message_handler;
    handlers["pong"] = Handlers::pong_handler;
    handlers["join"] = Handlers::channel_join_handler;

    if (LOG) std::cout << "CLIENT_LOG: Started app" << std::endl;
}

Client::~Client() {
    shutdown(hub_socket, SHUT_RDWR);
	close(hub_socket);

    if (LOG) std::cout << "CLIENT_LOG: Disconnected from HUB" << std::endl;
    if (LOG) std::cout << "CLIENT_LOG: Shutting Client down" << std::endl;
}