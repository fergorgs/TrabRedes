#include "Connection.h"

Connection::Connection(int s) : socket(s) {
    fcntl(socket, F_SETFL, O_NONBLOCK);
}; 