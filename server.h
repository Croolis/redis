#include <netinet/in.h>
#include <sys/select.h>
#include <vector>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

class server {
 private:
    int listener;
    struct sockaddr_in addr;
    fd_set socket_set;
    int max_sock;
    vector<int> socks;
    vector<string> reminder;
    vector<int> stop;

 public:
     server() {max_sock = 0;}

    int init_server() {
        FD_ZERO(&socket_set);
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

    void get_client() {
        cout << "try to accept" << endl;
        int sock = accept(listener, NULL, NULL);
        cout << "sock is " << sock << endl;
        FD_SET(sock, &socket_set);
        socks.push_back(sock);
        max_sock = max(sock, max_sock);
        reminder.push_back("");
        stop.push_back(0);
        cout << "added client correctly" << endl;
        this->get_client();
    }

    int get_sender() {
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        fd_set rdy_set = socket_set;
        cout << "start selecting\n";
        cout << "select is " << select(max_sock + 1, &rdy_set, NULL, NULL, &timeout) << endl;
        for (int i = 0; i < socks.size(); i++) {
            if (FD_ISSET(socks[i], &rdy_set))
                return i;
        }
        return -1;
    }

    int recive(string &msg, int &socket) {
        int sock_num = get_sender();
        if (sock_num == -1)
            return 0;
        cout << "Recive" << endl;
        socket = socks[sock_num];
        char buf[1024];
        if (recv(socket, buf, 1024, 0) <= 0) {
            swap(socks[sock_num], socks[socks.size() - 1]);
            socks.pop_back();
            FD_CLR(socket, &socket_set);
            return 0;
        }
        msg = buf;
        return 1;
    }

    /*int recive(string &msg, int &socket) {
        int sock = get_sender();
        socket = socks[sock];
        cout << "sock is " << sock << " socket with number " << socket << endl;
        if (sock == -1)
            return 1;
        if (stop[sock] == 1)
            return 1;
        char buf[1024];
        string s;
        int offset = 0;
        int working = 1;
        while (offset < reminder[sock].length() && reminder[sock][offset] <= '9' && reminder[sock][offset] >= '0') {
            s += reminder[sock][offset];
            offset++;
        }
        if (offset == reminder[sock].length()) {
            if (recv(socks[sock], buf, 1024, 0) == 0)
                stop[sock] = 1;
            reminder[sock] += buf;
            offset++;
            while (offset < reminder[sock].length() && reminder[sock][offset] <= '9' && reminder[sock][offset] >= '0') {
                s += reminder[sock][offset];
                offset++;
            }
        }
        if (reminder[sock].length() < 2) {
            stop[sock] = 1;
            return 1;
        }
        if (s == "") {
            stop[sock] = 1;
            return 1;
        }
        int t = stoi(s) * 2 + 1;
        offset = 0;
        while (t > 0) {
            if (offset + 1 <= reminder[sock].length() && reminder[sock][offset] == '\r' && reminder[sock][offset + 1] == '\n')
                t--;
            if (offset >= reminder[sock].length()) {
                if (recv(socks[sock], buf, 1024, 0) == 0)
                    stop[sock] = 1;
                reminder[sock] += buf;
            } else {
                offset++;
            }
        }
        string res, rem;
        for (int i = 0; i < offset; i++) {
            res += reminder[sock][i];
        }
        for (int i = offset; i < reminder[sock].length(); i++) {
            rem += reminder[sock][i];
        }
        reminder[sock] = rem;
        msg = res;
        return 1;
    }*/

    int respond(string msg, int sock_) {
        cout << "Responding" << endl;
        int len = msg.length();
        return send(sock_, msg.c_str(), len, 0);
    }

    ~server() {
        close(listener);
    }
};