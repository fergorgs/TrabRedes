#ifdef VS_HACK
    // This is not included on compilation, just in VS Code
    // to make IntelliSense work
    #include "PCHServer.h"
#endif

#include "Handlers.h"

void Handlers::say(Message* message, Hub* h, Connection* sender) {
    if (sender->cur_channel == nullptr) 
        return;

    bool senderIsMuted = false;

    for (auto& memb : sender->cur_channel->mutedMembers) {
        if(memb == sender) {
            senderIsMuted = true;
            break;
        }
    }

    if (!senderIsMuted) {
        std::size_t members = sender->cur_channel->members.size();

        MessageSendController* control = new MessageSendController(members);
        control->setBuffer(message->serializeMessage());
        int i = 0;
        for (auto& receiver : sender->cur_channel->members) {
            if (i++ < members) {
                receiver->write(control);
            } else {
                break;
            }
        }
    }
}

void Handlers::ping(Message* message, Hub* hub, Connection* sender) {
    Message* pong = new Message();
    
    pong->command.set_cmd("PONG");

    sender->send_msg(pong);

    delete pong;
}

void Handlers::nick(Message* message, Hub* hub, Connection* sender) {
    std::vector<std::string> params = message->params.getMiddleContent();
    
    if (params.size() == 1) {
        std::string new_nick = params[0];

        if(hub->nicks.find(new_nick) != hub->nicks.end()) {
            Message* nick_in_use = new Message();

            nick_in_use->command.set_cmd("433");
            nick_in_use->params.addMiddleParam(new_nick);
            nick_in_use->params.setTrailing("Nickname is already in use");

            sender->send_msg(nick_in_use);

            delete nick_in_use;

        } else {
            if (sender->nick != message->prefix.getNick())
                return;

            if(sender->nick.size() != 0) 
                hub->nicks.erase(sender->nick);
            
            hub->nicks[new_nick] = sender;
            sender->nick = new_nick;
            sender->send_msg(message);
        }
    }
    // else 461
}

void Handlers::confirm(Message* message, Hub* hub, Connection* sender) {
    sender->confirmReceive();
}

void Handlers::join(Message* message, Hub* hub, Connection* sender) {
    if (sender->nick != message->prefix.getNick())
        return;

    std::vector<std::string> params = message->params.getMiddleContent();

    if (params.size() == 1) {
        std::string name = params[0];
        
        if (sender->nick != message->prefix.getNick())
            return;

        if (name.size() != 0 && name[0] != '#') {
            Message* bad_mask = new Message();

            bad_mask->command.set_cmd("476");
            
            sender->send_msg(bad_mask);

            delete bad_mask;

        } else {
            Channel* channel = sender->cur_channel;

            if (channel) {
                channel->remove(sender);
                
                if (channel->members.empty()) {
                    hub->channels.erase(channel->name);
                    delete channel;
                }
            }
            
            if(hub->channels.find(name) == hub->channels.end()) 
                hub->channels[name] = new Channel(name, sender);

            hub->channels[name]->connect(sender);
            

            sender->send_msg(message);
        }
    } 
    // else 461
}

// continue here
void Handlers::kick(Message* m, Hub* h, Connection* sender) {
    Channel* channel = sender->cur_channel;

    if(channel != nullptr && channel->admin == sender) {
        std::vector<std::string> params = m->params.getMiddleContent();

        if (params.size() == 1) {
            std::string nick = params[0];

            Connection* kicked = channel->find(nick);

            if (kicked != nullptr) {
                channel->remove(kicked);

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