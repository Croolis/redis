#include <string>
#include <vector>
#include <iostream>
#include "parser.h"
#include "container.h"

using namespace std;

class handler {
 private:
    parser pars_;
    container *cont_;

    string set(vector<string> req) {
        cout << "SET: " << req[1] << ' ' << req[2] << endl;
        cont_->set_val(req[1], req[2]);
        return pars_.encodeSimpleString("OK");
    }

    string get(vector<string> req) {
        cout << "GET: " << req[1] << ' ';
        string value = cont_->get_val(req[1]);
        cout << value << endl;
        return pars_.encodeBulkString(value);
    }

    string del(vector<string> req) {
        cout << "DEL: " << req[1] << endl;
        cont_->del_val(req[1]);
        return pars_.encodeSimpleString("OK");
    }

    string ping(vector<string> req) {
        cout << "PING" << endl;
        return pars_.encodeSimpleString("PONG");
    }

    string expire(vector<string> req) {
        cout << "EXPIRE: " << req[1] << endl;
        int res = cont_->set_ttl(req[1], stoll(req[2], NULL));
        return pars_.encodeInteger(res);
    }

    string save(vector<string> req) {
        cout << "SAVE" << endl;
        if (req.size() < 2) {
            req.push_back("last");
        }
        cont_->save_screen(req[1]);
        return pars_.encodeSimpleString("OK");
    }

    string load(vector<string> req) {
        cout << "LOAD" << endl;
        if (req.size() < 2) {
            req.push_back("last");
        }
        cont_->load_screen(req[1]);
        return pars_.encodeSimpleString("OK");
    }

    string incorrect_message(vector<string> req) {
        cout << "Incorrect message" << endl;
        return pars_.encodeError("Error in pasring your message");
    }

    string unknown_command(vector<string> req) {
        cout << "Unknown command" << endl;
        return pars_.encodeError("Unknown command");
    }

    string save_screen(vector<string> req) {
        cont_->save_screen(req[1]);
        cout << "Save screen to file " << req[1] << endl; 
        return pars_.encodeSimpleString("OK");
    }

    string load_screen(vector<string> req) {
        cont_->load_screen(req[1]);
        cout << "Load screen from file " << req[1] << endl; 
        return pars_.encodeSimpleString("OK");
    }    
 public:
    handler() {
        cont_ = new container();
    }

    string handle(string msg) {
        cout << "Handling" << endl;
        vector<string> request = pars_.decode(msg);
        if (request.size() == 0) {
            cout << "Error in decoding" << endl;
            return this->incorrect_message(request);
        }
        if (request[0] == "ping") {
            return this->ping(request);
        }          
        if (request[0] == "set") {
            if (request.size() >= 3)
                return this->set(request);
        }
        if (request[0] == "get") {
            if (request.size() >= 2)
                return this->get(request);
        }
        if (request[0] == "del") {
            if (request.size() >= 2)
                return this->del(request);
        }
        if (request[0] == "expire") {
            if (request.size() >= 3)
                return this->expire(request);
        }
        if (request[0] == "save") {
            return this->save(request);
        }
        if (request[0] == "load") {
            return this->load(request);
        }
        if (request[0] == "Incorrect message") {
            return this->incorrect_message(request);
        }
        return this->unknown_command(request);
    }

    void clean() {
        cont_->del_ttl();
    }
};