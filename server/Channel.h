
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
        Connection* remove(Connection*); // remove by reference
        Connection* remove(std::string); // remove by nick
        // Connection* remove(int);         // remove by socket (?needed?)
        Connection* find(std::string);
    };


#endif