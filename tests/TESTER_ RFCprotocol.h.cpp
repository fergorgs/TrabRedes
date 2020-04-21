#include <stdio.h>
#include <stdlib.h>

#include "RFCprotocol.h"

int main(){

//TEST STRING PARSING FUNCTION
    // string msg = "Eu gosto de muita batata doce!";

    // vector<string> strs = parseString(msg, " ");

    // for(int i = 0; i < strs.size(); i++)
    //     cout << "||" << strs[i] << "||" << endl;


//TESTE SERIALIZED --> UNSERIALIZED
    
    cout << "TESTE SERIALIZED --> UNSERIALIZED" << endl << endl;

    string msg = ":amy!Amy123@foo.example.com 001 amy :Test messege";

    Messege m1 = Messege(msg);

    m1.listMessegeComponents();

//TESTE UNSERIALIZED --> SERIALIZED

    cout << endl << endl << "TESTE UNSERIALIZED --> SERIALIZED" << endl << endl;

    Messege m2 = Messege();

    m2.prefix.setNick("amy");
    m2.prefix.setUser("Amy123");
    m2.prefix.setHost("foo.example.com");

    //m2.command.setNumber(123);
    m2.command.setWord("thisIsTheCommand");

    m2.params.addMiddleParam("amy");
    m2.params.setTrailing("Test messege");

    m2.listMessegeComponents();

    cout << endl;

    string msg2 = m2.serializeMessege();

    cout << "Serialized messege: " << msg2 << endl;
}