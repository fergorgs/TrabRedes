#ifndef HANDLERS_H
#define HANDLERS_H

#include "Client.h"
#include "../utils/RFCprotocol.h"

namespace Handlers {
    void nickname_in_use_handler(Client* client, Message* msg);
    void nickname_change_handler(Client* client, Message* msg);
    void message_handler(Client* client, Message* msg);
    void pong_handler(Client* client, Message* msg);
    void channel_join_handler(Client* client, Message* msg);
    void channel_op_privs_needed_handler(Client* client, Message* msg);
    void kicked_from_channel_handler(Client* client, Message* msg);
    void who_is_response_handler(Client* client, Message* msg);
}

#endif