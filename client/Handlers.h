#ifndef HANDLERS_H
#define HANDLERS_H

#include "Client.h"
#include "../utils/RFCprotocol.h"

namespace Handlers {
    void nickname_in_use_handler(Client* client, Message* msg);
    void nickname_change_handler(Client* client, Message* msg);
    void message_handler(Client* client, Message* msg);
    void pong_handler(Client* client, Message* msg);
}

#endif