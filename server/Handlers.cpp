#include <iostream>
#include "Handlers.h"

void Handlers::say(Message* m, Hub* h, Connection* sender) {
    if(sender->cur_channel == nullptr) return;

    bool senderIsMuted = false;

    for(auto& memb : sender->cur_channel->mutedMembers)
        if(memb == sender)
            senderIsMuted = true;

    if(!senderIsMuted){
        int n = sender->cur_channel->members.size();
        MessageSendController* msc = new MessageSendController(n);
        msc->setBuffer(m->serializeMessage());
        int i = 0;
        for(auto& s : sender->cur_channel->members) if(i++ < n) s->write(msc);
    }
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
    
    if(h->channels.find(name) == h->channels.end()) 
        h->channels[name] = new Channel(name, sender);

    h->channels[name]->connect(sender);
    

    sender->send_msg(m);
}

void Handlers::kick(Message* m, Hub* h, Connection* sender) {
    if(sender->cur_channel && sender->cur_channel->admin == sender) {
        std::vector<std::string> nicks = m->params.getMiddleContent();

        for (std::string& nick : nicks) {
            Connection* kicked = sender->cur_channel->remove(nick);

            if (kicked != nullptr) {
                Message* kick = new Message();

                kick->command.set_cmd("KICK");
                kick->params.setTrailing(m->params.getTrailing());

                kicked->send_msg(kick);

                delete kick;
            }
        }
    } else {
        Message* error_op = new Message();

        error_op->command.set_cmd("482");
        
        sender->send_msg(error_op);

        delete error_op;
    }
}

void Handlers::whois(Message* message, Hub* server, Connection* sender) {
    if (sender->cur_channel && sender->cur_channel->admin == sender) {
        std::vector<std::string> params = message->params.getMiddleContent();

        if (params.size() != 0) {
            std::string nick = params[0];

            Connection* who = sender->cur_channel->find(nick);

            if (who != nullptr) {
                message->params.setTrailing(who->ip_addr);

                sender->send_msg(message);
            }
        }
    } else {
        Message* error_op = new Message();

        error_op->command.set_cmd("482");
        
        sender->send_msg(error_op);

        delete error_op;
    }
}

void Handlers::mute(Message* m, Hub* h, Connection* sender) {
    if(sender->cur_channel && sender->cur_channel->admin == sender) {
        
        std::vector<std::string> nicks = m->params.getMiddleContent();

        for (std::string& nick : nicks) {
            
            if(sender->cur_channel->find(nick) != nullptr){
                sender->cur_channel->mute(sender->cur_channel->find(nick));
                
                //Connection* kicked = sender->cur_channel->remove(nick);

                //if (kicked != nullptr) {
                    Message* mute = new Message();
                    mute->command.set_cmd("MUTE");
                    mute->params.addMiddleParam(nick);
                    sender->send_msg(mute);

                    Message* mutewarn = new Message();
                    mutewarn->command.set_cmd("MUTEWARN");
                    //unmutewarn->params.addMiddleParam(nick);
                    (sender->cur_channel->find(nick))->send_msg(mutewarn);

                    delete mute;
                    delete mutewarn;
                //}
            }
        }
    } else {
        Message* error_op = new Message();

        error_op->command.set_cmd("482");
        
        sender->send_msg(error_op);

        delete error_op;
    }
}

void Handlers::unmute(Message* m, Hub* h, Connection* sender) {
    if(sender->cur_channel && sender->cur_channel->admin == sender) {
        
        std::vector<std::string> nicks = m->params.getMiddleContent();

        for (std::string& nick : nicks) {
            
            if(sender->cur_channel->find(nick) != nullptr){
                sender->cur_channel->unmute(sender->cur_channel->find(nick));
                
                //Connection* kicked = sender->cur_channel->remove(nick);

                //if (kicked != nullptr) {
                    Message* unmute = new Message();
                    unmute->command.set_cmd("UNMUTE");
                    unmute->params.addMiddleParam(nick);
                    sender->send_msg(unmute);

                    Message* unmutewarn = new Message();
                    unmutewarn->command.set_cmd("UNMUTEWARN");
                    //unmutewarn->params.addMiddleParam(nick);
                    (sender->cur_channel->find(nick))->send_msg(unmutewarn);

                    delete unmute;
                    delete unmutewarn;
                //}
            }
        }
    } else {
        Message* error_op = new Message();

        error_op->command.set_cmd("482");
        
        sender->send_msg(error_op);

        delete error_op;
    }
}