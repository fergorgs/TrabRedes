#include "Handlers.h"

void Handlers::say(Message* m, Hub* h, Connection* sender) {
    int n = h->connections.size();
    MessageSendController* msc = new MessageSendController(n);
    msc->setBuffer(m->params.getTrailing());
    int i = 0;
    for(auto& s : h->connections) if(i++ < n) s->write(msc);
}
void Handlers::ping(Message* m, Hub* h, Connection* sender) {
    sender->pong();
};
void Handlers::nick(Message* m, Hub* h, Connection* sender) {
    std::string s = m->prefix.getNick();
    if(h->nicks.find(s) != h->nicks.end()) {
        Message* nm = new Message();
        nm->command.setWord("433");
        sender->send_msg(nm);
        delete nm;
    } else {
        if(sender->nick.size()) h->nicks.erase(sender->nick);
        h->nicks[s] = sender;
        sender->nick = s;
        sender->send_msg(m);
    }
};
