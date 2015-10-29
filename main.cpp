#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include "server.h"

using namespace std;

int main() {
    server serv;
    serv.init_server();
    client c = serv.get_client();
    char mes[1024];
    int len = 1024;
    len = c.recive(mes, len);
    mes[len] = '\0';
    c.respond(mes, len);
}