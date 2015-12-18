#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include <time.h>
#include <thread>
#include "server.h"
#include "handler.h"

using namespace std;

int main() {
    server serv;
    if (serv.init_server())
        return 0;
    handler handler_;
    long long last_clean = 0;
    int len = 1024;
    thread tr([&serv](){serv.get_client();});
    while (true) {
        string msg;
        int sock;
        if (serv.recive(msg, sock) > 0) {
            string resp = handler_.handle(msg);
            serv.respond(resp, sock);
            if (time(NULL) - last_clean < 10) {
                handler_.clean();
                last_clean = time(NULL);
            }
        }
    }
}
