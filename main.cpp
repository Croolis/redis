#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include "server.h"
#include "container.h"
#include "parser.h"

using namespace std;

int main() {
    server serv;
    serv.init_server();
    parser p;
    container cont;
    string msg;
    int len = 1024;
    while (true) {
        client c = serv.get_client();
        while (c.recive(msg) > 0) {
            vector<string> request = p.decode(msg);
            if (request[0] == "get") {
                string value = cont.get(request[1]);
                cout << value << endl;
                string resp = p.encodeBulkString(value);
                len = resp.length();
                c.respond(resp, len);
                continue;
            }
            if (request[0] == "set") {
                cout << request[1] << ' ' << request[2] << endl;
                cont.set(request[1], request[2]);
                string resp = p.encodeSimpleString("OK");
                len = resp.length();
                c.respond(resp, len);
                continue;
            }
            if (request[0] == "ping") {
                cout << "ping";
                string resp = p.encodeSimpleString("PONG");
                len = resp.length();
                c.respond(resp, len);
                continue;
            }            
            if (request[0] == "Incorrect message") {
                cout << "error in decoding message at symbol " << request[1] << endl;
                continue;
            }
            cout << "unknown command: " << request[0] << endl;
        }
    }
}
