#ifndef HANDLERS_H
#define HANDLERS_H

#include <list>
#include "Connection.h"
#include "MessageSendController.h"
#include "../utils/RFCprotocol.h"
#include "Hub.h"

class Hub;

namespace Handlers {
    void say(Message*, Hub*, Connection*);
    void ping(Message*, Hub*, Connection*);
    void nick(Message*, Hub*, Connection*);
    void confirm(Message*, Hub*, Connection*);
};

#endif // HANDLERS_H