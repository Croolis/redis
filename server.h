#include <netinet/in.h>
#include "client.h"

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
        // TODO: Do smth with incorrect value of socket
        //if (sock < 0)
        //    return NULL;
        client c(sock);
        return c;
    }

    ~server() {
        close(listener);
    }
};