#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include <time.h>
#include "server.h"
#include "handler.h"

using namespace std;

int main() {
    server serv;
    serv.init_server();
    handler handler_;
    long long last_clean = 0;
    int len = 1024;
    while (true) {
        string msg;
        client c = serv.get_client();
        while (c.recive(msg) > 0) {
            string resp = handler_.handle(msg);
            c.respond(resp);
            if (time(NULL) - last_clean < 10) {
                handler_.clean();
                last_clean = time(NULL);
            }
        }
    }
}
