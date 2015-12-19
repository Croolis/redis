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
    vector<string> reminder;
    vector<int> socks;
    vector<int> state;
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
        int sock = accept(listener, NULL, NULL);
        FD_SET(sock, &socket_set);
        socks.push_back(sock);
        max_sock = max(sock, max_sock);
        reminder.push_back("");
        stop.push_back(0);
        state.push_back(0);
        this->get_client();
    }

    void remove_client(int sock_num) {
        int socket = socks[sock_num];
        swap(socks[sock_num], socks[socks.size() - 1]);
        socks.pop_back();
        swap(reminder[sock_num], reminder[reminder.size() - 1]);
        reminder.pop_back();
        swap(state[sock_num], state[state.size() - 1]);
        state.pop_back();
        swap(stop[sock_num], stop[stop.size() - 1]);
        stop.pop_back();
        FD_CLR(socket, &socket_set);
    }

    int get_sender() {
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        fd_set rdy_set = socket_set;
        select(max_sock + 1, &rdy_set, NULL, NULL, &timeout);
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
        cout << sock_num << endl;
        cout << "Recive" << endl;
        socket = socks[sock_num];
        cout << socket << endl;
        char buf[1024];
        string mes = reminder[sock_num];
        int size = 0, pointer = 0, start;
        state[sock_num] = -1;
        while (state[sock_num] == -1) {
            if (pointer >= mes.size()) {
                if (recv(socket, buf, 1024, 0) <= 0) {
                    remove_client(sock_num);
                    close(socket);
                    return 0;
                }
                mes += string(buf);
                continue;
            }
            if (mes[pointer] == '*') {
                start = pointer;
                state[sock_num]++;
            }
            pointer++;
        }
        while (state[sock_num] == 0) { // read array size
            if (pointer >= mes.size()) {
                if (recv(socket, buf, 1024, 0) <= 0) {
                    remove_client(sock_num);
                    close(socket);
                    return 0;
                }
                mes += string(buf);
                continue;
            }
            if (mes[pointer] <= '9' && mes[pointer] >= '0') {
                size = size * 10 + mes[pointer] - '0';
                pointer++;
            }
            else
                state[sock_num]++;
        }
        pointer += 2; // skip '/r/n'

        int length = 0;
        while (size > 0) { // read SIZE strings
            if (pointer >= mes.size()) {
                if (recv(socket, buf, 1024, 0) <= 0) {
                    remove_client(sock_num);
                    close(socket);
                    return 0;
                }
                mes += string(buf);
                continue;
            }
            if (state[sock_num] == 1) { // read length
                if (mes[pointer] == '$') {
                    pointer++;
                    continue;
                }
                if (mes[pointer] <= '9' && mes[pointer] >= '0') {
                    length = length * 10 + mes[pointer] - '0';
                    pointer++;
                }
                else {
                    state[sock_num]++;
                    pointer += 2; // skip '/r/n'
                }
                continue;
            }
            if (state[sock_num] == 2) { // read string
                pointer += length; // skip LENGTH chars
                pointer += 2; // skip '/r/n'
                while (pointer > mes.size()) {
                    if (recv(socket, buf, 1024, 0) <= 0) {
                        remove_client(sock_num);
                        close(socket);
                        return 0;
                    }
                    mes += string(buf);
                }
                state[sock_num]--;
                length = 0;
                size--;
                continue;
            }
        }

        if (size != 0)
            return 0;

        msg = "";
        for (int i = start; i < pointer; i++)
            msg += mes[i];

        reminder[sock_num] = "";
        for (int i = pointer; i < mes.length(); i++)
            reminder[sock_num] += mes[i];
        return 1;
    }

    int respond(string msg, int sock_) {
        cout << "Responding" << endl;
        int len = msg.length();
        return send(sock_, msg.c_str(), len, 0);
    }

    ~server() {
        close(listener);
    }
};