#include <string>
#include <vector>
#include <iostream>

using namespace std;

class parser {
 public:
    vector<string> decode(string ms) {
        string msg = ms + "\n";
        vector<string> res;
        vector<string> error;
        error.push_back("Incorrect message");
        string s = "";
        if (msg[0] == '*') {
            int offset = 1;
            while (offset < msg.length() && msg[offset] <= '9' && msg[offset] >= '0') {
                s += msg[offset];
                offset++;
            }
            if (offset + 1 >= msg.length() || msg[offset] != '\r' || msg[offset + 1] != '\n'){            
                error.push_back(to_string(offset));
                return error;
            }
            offset += 2;
            if (s == "") {
                error.push_back(to_string(offset));
                return error;
            }
            int size = stoi(s);
            for (int counter = 0; counter < size; counter++) {
                if (offset >= msg.length() || msg[offset] != '$') {
                    error.push_back(to_string(offset));
                    return error;
                }
                offset++;
                s = "";
                while (offset < msg.length() && msg[offset] <= '9' && msg[offset] >= '0') {
                    s += msg[offset];
                    offset++;
                }
                if (s == "") {
                    error.push_back(to_string(offset));
                    return error;
                }
                int len = stoi(s);
                if (offset + 1 >= msg.length() || msg[offset] != '\r' || msg[offset + 1] != '\n'){
                    error.push_back(to_string(offset));
                    return error;
                }
                offset += 2;
                string st = "";
                for (int j = 0; j < len && offset < msg.length(); j++) {
                    st += msg[offset];
                    offset++;
                }
                if (offset + 1 >= msg.length() || msg[offset] != '\r' || (msg[offset + 1] != '\n' && msg[offset + 1] != '\0')){
                    error.push_back(to_string(offset));
                    return error;
                }
                offset += 2;
                res.push_back(st);
            }            
            return res;
        }
    }

    string encodeSimpleString(string msg) {
        return "+" + msg + "\r\n";
    }

    string encodeInteger(int msg) {
        string t = to_string(msg);
        return ":" + t + "\r\n";
    }

    string encodeError(string msg) {
        return "-" + msg + "\r\n";
    }

    string encodeBulkString(string msg) {
        string res = "$";
        if (msg.length() > 0) {
            res += to_string(msg.length());
            res += "\r\n";
            res += msg;
        } else {
            res += "-1";
        }
        res += "\r\n";
        return res;
    }

    string encodeBulkArr(vector<string> msg) {
        string res = "*";
        res += to_string(msg.size());
        res += "\r\n";
        for (int i = 0; i < msg.size(); i++) {
            res += encodeBulkString(msg[i]);
        }
        return res;
    }
};