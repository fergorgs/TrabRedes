#include <iostream>
#include "Handlers.h"

void Handlers::say(Message* m, Hub* h, Connection* sender) {
    if(!sender->cur_channel) return;
    int n = sender->cur_channel->members.size();
    MessageSendController* msc = new MessageSendController(n);
    msc->setBuffer(m->serializeMessage());
    int i = 0;
    for(auto& s : sender->cur_channel->members) if(i++ < n) s->write(msc);
}

void Handlers::ping(Message* m, Hub* h, Connection* sender) {
    sender->pong();
}

void Handlers::nick(Message* m, Hub* h, Connection* sender) {
    std::string s = m->params.getTrailing();
    if(h->nicks.find(s) != h->nicks.end()) {
        Message* nm = new Message();
        nm->command.set_cmd("433");
        sender->send_msg(nm);
        delete nm;
    } else {
        if(sender->nick.size()) h->nicks.erase(sender->nick);
        h->nicks[s] = sender;
        sender->nick = s;
        sender->send_msg(m);
    }
}

void Handlers::confirm(Message* m, Hub* h, Connection* sender) {
    sender->confirmReceive();
}

void Handlers::join(Message* m, Hub* h, Connection* sender) {
    std::string name = m->params.getTrailing();
    Channel* c = sender->cur_channel;
    if(c) {
        c->remove(sender);
        if(c->members.empty()) {
            h->channels.erase(c->name);
            delete c;
        }
    }
    if(h->channels.find(name) != h->channels.end()) 
        h->channels[name]->connect(sender);
    else 
        h->channels[name] = new Channel(name, sender);    
}

void Handlers::kick(Message* m, Hub* h, Connection* sender) {
    if(sender->cur_channel && sender->cur_channel->admin == sender) 
        sender->cur_channel->remove(m->params.getTrailing());
}