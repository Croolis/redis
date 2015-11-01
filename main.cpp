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
    char m[1024];
    int len = 1024;
    while (true) {
        client c = serv.get_client();
        
        while (c.recive(m, len) > 0) {
            string msg = m;
            vector<string> request = p.decode(msg);
            if (request[0] == "get") {
                string value = cont.get(request[1]);
                cout << value << endl;
                len = value.length();
                char resp_str[len + 1];
                strcpy(resp_str, value.c_str());
                c.respond(resp_str, len);
            }
            if (request[0] == "post") {
                cout << request[1] << ' ' << request[2] << endl;
                cont.set(request[1], request[2]);
            }
            if (request[0] == "Incorrect message") {
                cout << "error in decoding message at symbol " << request[1] << endl;
            }
        }
    }
}
