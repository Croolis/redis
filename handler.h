#include <string>
#include <vector>
#include <iostream>
#include "parser.h"
#include "container.h"

using namespace std;

class handler {
 private:
    parser pars_;
    container cont_;

    string set(vector<string> req) {
        cont_.set(req[1], req[2]);
        cout << req[1] << ' ' << req[2] << endl;
        return pars_.encodeSimpleString("OK");
    }

    string get(vector<string> req) {
        string value = cont_.get(req[1]);
        cout << value << endl;
        return pars_.encodeBulkString(value);
    }

    string del(vector<string> req) {
        cont_.del(req[1]);
        cout << req[1] << endl;
        return pars_.encodeSimpleString("OK");
    }

    string ping(vector<string> req) {
        cout << "ping" << endl;
        return pars_.encodeSimpleString("PONG");
    }

    string incorrect_message(vector<string> req) {
        cout << "incorrect message" << endl;
        return pars_.encodeError("Error in pasring your message");
    }

    string unknown_command(vector<string> req) {
        cout << "Unknown command" << endl;
        return pars_.encodeError("Unknown command");
    }
 public:
    string handle(string msg) {
        vector<string> request = pars_.decode(msg);
        if (request[0] == "ping") {
            return this->ping(request);
        }          
        if (request[0] == "set") {
            return this->set(request);
        }
        if (request[0] == "get") {
            return this->get(request);
        }
        if (request[0] == "delete") {
            return this->del(request);
        }
        if (request[0] == "Incorrect message") {
            return this->incorrect_message(request);
        }
        return this->unknown_command(request);
    }
};