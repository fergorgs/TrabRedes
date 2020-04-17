#include <string>
#include <vector>
#include <ctype.h>
#include <iostream>

using namespace std;

#define NUL 0
#define LF 10
#define CR 13
#define SPACE 32

vector<string> parseString(string original, string del);


class Prefix{

    private:
    
    string serverName;
    string nick;
    string user;
    string host;

    public:

    Prefix();

    int setServerName(string serverName);
    int setNick(string nick);
    int setUser(string user);
    int setHost(string host);

    string getServerName();
    string getNick();
    string getUser();
    string getHost();

};


class Command{

    private:

    string word;
    int number;

    public:

    Command();

    int setWord(string word);
    int setNumber(int number);

    string getWord();
    int getNumber();
};


class Param{

    private:

    vector<string> middle;
    string trailing;

    public:

    Param();

    int addMiddleParam(string param);
    int setTrailing(string trailing);

    vector<string> getMiddleContent();
    string getTrailing();
};


class Messege{

    public:
    
    Prefix prefix;
    Command command;
    Param params;

    Messege();

    Messege(string serializedMessege);
    
    string serializeMessege();

    void listMessegeComponents();
};
