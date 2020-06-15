#include "Channel.h"
#include <iostream>

Channel::Channel(std::string n, Connection* adm ) : name(n), admin(adm)  { };

Channel::~Channel() {
    std::cout << "Channel destructed";
}

void Channel::unmute(Connection* member) {
    // if(find(whitelist.begin(), whitelist.end(), member) != whitelist.end()) 
    mutedMembers.remove(member);
}

void Channel::mute(Connection* member) {
    // if(find(whitelist.begin(), whitelist.end(), member) != whitelist.end()) 
    mutedMembers.push_back(member);
}

void Channel::connect(Connection* member) {
    // if(find(whitelist.begin(), whitelist.end(), member) != whitelist.end()) 
    members.push_back(member);
    member->cur_channel = this;
    member->channel_pos = --members.end(); // (error?) - 1; // iterator to member's position in members => O(1) quit
}

Connection* Channel::remove(std::string s) {
    auto it = find_if(members.begin(), members.end(), [&](Connection* m) {return m->nick == s;});
    if(it != members.end()) {
        Connection* member = *it;
        members.erase(it);
        member->cur_channel = nullptr;

        return member;
    }

    return nullptr;
}

// Connection* Channel::remove(int socket) {
//     auto it = find_if(members.begin(), members.end(), [&](Connection* m) {return m->socket == socket;});
//     if(it != members.end()) {
//         Connection* member = *it;
//         members.erase(it);
//         member->cur_channel = nullptr;

//         return member;
//     }

//     return nullptr;
// }

Connection* Channel::remove(Connection* member) {
    if(member->cur_channel == this) {
        members.erase(member->channel_pos);
        member->cur_channel = nullptr;

        return member;
    }

    return nullptr;
}

Connection* Channel::find(std::string nick) {
    auto it = find_if(members.begin(), members.end(), [&](Connection* m) {return m->nick == nick;});

    if (it != members.end()) 
        return *it;
        
    return nullptr;
}