class client {
 private:
    int sock_;

 public:
    client(int sock) : sock_(sock) {}

    int recive(char *msg, int &msg_len) {
        return recv(sock_, msg, msg_len, 0);
    }
    // TODO: Make it read and write whole message if (size of message > size of buffer)
    int respond(char *msg, int &msg_len) {
        return send(sock_, msg, msg_len, 0);
    }

    ~client() {
        close(sock_);
    }
};