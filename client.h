#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class client {
 private:
    string reminder_;
    int sock_;
    int stop_;

 public:
    client(int sock) : sock_(sock), stop_(0) {}

    int recive(string &msg) {
        if (stop_ == 1)
            return 0;
        char buf[1024];
        string s;
        int offset = 0;
        int working = 1;
        while (offset < reminder_.length() && reminder_[offset] <= '9' && reminder_[offset] >= '0') {
            s += reminder_[offset];
            offset++;
        }
        if (offset == reminder_.length()) {
            if (recv(sock_, buf, 1024, 0) == 0)
                stop_ = 1;
            reminder_ += buf;
            offset++;
            while (offset < reminder_.length() && reminder_[offset] <= '9' && reminder_[offset] >= '0') {
                s += reminder_[offset];
                offset++;
            }
        }
        if (reminder_.length() < 2) {
            stop_ = 1;
            return 0;
        }
        if (s == "") {
            stop_ = 1;
            return 0;
        }
        int t = stoi(s) * 2 + 1;
        offset = 0;
        while (t > 0) {
            if (offset + 1 <= reminder_.length() && reminder_[offset] == '\r' && reminder_[offset + 1] == '\n')
                t--;
            if (offset >= reminder_.length()) {
                if (recv(sock_, buf, 1024, 0) == 0)
                    stop_ = 1;
                reminder_ += buf;
            } else {
                offset++;
            }
        }
        string res, rem;
        for (int i = 0; i < offset; i++) {
            res += reminder_[i];
        }
        for (int i = offset; i < reminder_.length(); i++) {
            rem += reminder_[i];
        }
        reminder_ = rem;
        msg = res;
        return 1;
    }
    // TODO: Make it write whole message if (size of message > size of buffer)
    int respond(string msg) {
        int len = msg.length();
        return send(sock_, msg.c_str(), len, 0);
    }

    ~client() {
        close(sock_);
    }
};