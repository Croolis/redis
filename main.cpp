#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include "server.h"
#include "handler.h"

using namespace std;

int main() {
    server serv;
    serv.init_server();
    handler handler_;
    int len = 1024;
    while (true) {
        string msg;
        client c = serv.get_client();
        while (c.recive(msg) > 0) {
            string resp = handler_.handle(msg);
            c.respond(resp);
        }
    }
}
