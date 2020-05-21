#include <string>

#ifndef MESSAGE_H
#define MESSAGE_H

class Message {
    private:
        std::string buffer;
        int clients;
    public:
        const char* getBuffer();
        void setBuffer(char*);
        void deduct();
        Message(int);
        ~Message();
};

#endif // MESSAGE_H