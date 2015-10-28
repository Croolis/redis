#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class server {
 private:
    int listener;
    struct sockaddr_in addr;

 public:    
    server() {}
    
    int init_server() {
        listener = socket(AF_INET, SOCK_STREAM, 0);
        if(listener < 0)
        {
            perror("socket");
            return 1;
        }
        
        addr.sin_family = AF_INET;
        addr.sin_port = htons(6379);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("bind");
            return 2;
        }

        listen(listener, 8);
        return 0;
    }

    client get_client() {
        int sock = accept(listener, NULL, NULL);
        if sock < 0
            return NULL;
        client c(sock, this);
        return c;
    }

    int get_msg(char *buf, int &bytes_read) {
        bytes_read = recv(sock, buf, 1024, 0);
        if(bytes_read <= 0) {
            return 0;
        }
        return 1;
    }
    
    int send_msg(char *buf, int bytes_send) {
        return send(sock, buf, bytes_send, 0);
    }

    /*while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            return(3);
        }

        while(1)
        {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) break;
            send(sock, buf, bytes_read, 0);
        }
    
        close(sock);
    }*/
};

class client {
 private:
    int sock_;
    server *s_;
 
 public:
    client(int sock, server *s) : sock_(sock), s_(s) {}

    void recive(char *msg, int &msg_len) {
        s.get_msg(msg, msg_len);
    }
    // TODO: Make it read and write whole message if (size of message > size of buffer)
    void send(char *msg, int &msg_len) {
        s.send_msg(msg, msg_len);
    }

};

int main() {
    server serv();
    serv.init_server();
    client c = serv.get_client();
    char mes[1024];
    int len = 1024;
    cout << mes << endl;
    c.recive(mes, len);
    cout << mes << endl;
}