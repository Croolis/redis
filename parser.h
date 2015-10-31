#include <string>
#include <vector>
#include <iostream>

using namespace std;

class parser {
 public:
    vector<string> decode(string msg) {
        vector<string> res;
        vector<string> error;
        error.push_back("Incorrect message");
        string s = "";
        int offset = 0;
        while (offset < msg.length() && msg[offset] <= '9' && msg[offset] >= '0') {
            s += msg[offset];
            offset++;
        }
        if (offset + 1 >= msg.length() || msg[offset] != '\r' || msg[offset + 1] != '\n'){            
                error.push_back(to_string(offset));
                return error;
            }
        offset += 2;
        stoi(s);
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
            if (offset + 1 >= msg.length() || msg[offset] != '\r' || msg[offset + 1] != '\n'){
                error.push_back(to_string(offset));
                return error;
            }
            offset += 2;
            res.push_back(st);            
        }
        return res;
    }

    string encode(vector<string> msg) {
        string res;
        res += to_string(msg.size()) + "\r\n";
        for (int i = 0; i < msg.size(); i++) {
            res += "$";
            res += to_string(msg[i].length());
            res += "\r\n";
            res += msg[i];
            res += "\r\n";
        }
        return res;
    }
};