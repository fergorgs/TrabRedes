
#include <list>
#include <string>


#ifndef CHANNEL_H
#define CHANNEL_H

#include "Connection.h"
class Connection;

class Channel {
    private:
        std::list<Connection*> whitelist;
    public:
        std::string name;
        Connection* admin;
        std::list<Connection*> members;
        Channel(std::string, Connection*);
        ~Channel();
        void connect(Connection*); 
        void remove(Connection*); // remove by reference
        void remove(std::string); // remove by nick
        void remove(int);         // remove by socket (?needed?)
    };


#endif