#include <string>
#include <set>
#include <unordered_map>
#include <time.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class container {
 private:
    unordered_map<string, string> container_;
    unordered_map<string, time_t> ttl_;
    set<pair<string, time_t> > ttl_set_;
    ofstream log;

    void set_(const string &key, const string &value) {
        container_[key] = value;
        ttl_.erase(key);
        auto it = ttl_set_.lower_bound(make_pair(key, 0));
        if (it != ttl_set_.end())
            it++;
        if (it != ttl_set_.end() && it->first == key)
            ttl_set_.erase(it);
    }

    void del_(const string &key) {
        container_.erase(key);
        ttl_.erase(key);
        auto it = ttl_set_.lower_bound(make_pair(key, 0));
        it++;
        if (it != ttl_set_.end() && it->first == key)
            ttl_set_.erase(it);
    }

    int set_ttl_(const string &key, const long long &ttl) {
        if (container_.count(key) == 0)
            return 0;
        ttl_[key] = ttl;
        ttl_set_.insert(make_pair(key, ttl));
        return 1;
    }

    void load_from_file(const string &name) {
        container_.clear();
        int fd = open(name.c_str(), O_RDONLY);
        if (fd == -1) {
            cout << "Can not open " << name << endl;
        } else {
            char str[4096];
            int len = 0, p = -1, num = 0, condition = 0, command = 0;
            string key, value;
            while (true) {
                p++;
                if (p >= len) {
                    len = read(fd, str, sizeof(str));
                    if (len < 0)
                        cout << "Error in reading " << name << endl;
                    p = 0;
                }
                if (len <= 0)
                    break;
                if (condition == 0) {
                    if (str[p] == '#') {
                        condition = (condition + 1) % 5;
                        continue;
                    }
                    else {
                        command *= 10;
                        command += str[p] - '0';
                    }                     
                }
                if (condition == 1 || condition == 3) {
                    if (str[p] == '$') {
                        condition = (condition + 1) % 5;
                        continue;
                    }
                    else {
                        num *= 10;
                        num += str[p] - '0';
                    }           
                }
                if (condition == 2) {
                    key += str[p];
                    num--;
                    if (num == 0) {
                        condition = (condition + 1) % 5;
                        continue;
                    }
                }
                if (condition == 4) {
                    value += str[p];
                    num--;
                    if (num == 0) {
                        condition = (condition + 1) % 5;
                        if (command == 1)
                            this->set_(key, value);
                        if (command == 2)
                            this->del_(key);
                        if (command == 3)
                            this->set_ttl_(key, stoll(value));
                        key = "";
                        value = "";
                        command = 0;
                        continue;
                    }
                }
            }
            close(fd);
        }
    }

 public:
    container() {
        load_from_file("log.t");
        log.open("log.t");
        auto t = container_.begin();
        time_t cur_time = time(NULL);
        while (t != container_.end()) {
            if (t->first != "" || t->second != "") {
                log << "1#" << t->first.length() << '$' << t->first << t->second.length() << '$' << t->second;
                log.flush();
                if ((ttl_.count(t->first)) > 0 && (ttl_[t->first] > cur_time)) {
                    log << "3#" << t->first.length() << '$' << t->first << to_string(ttl_[t->first]).length() << '$' << ttl_[t->first];
                    log.flush();
                }
            }
            t++;
        }
    }

    void save_screen(const string &name) {
        ofstream screen_file;
        screen_file.open(name + ".t");
        auto t = container_.begin();
        time_t cur_time = time(NULL);
        while (t != container_.end()) {
            if (t->first != "" || t->second != "") {
                log << "1#" << t->first.length() << '$' << t->first << t->second.length() << '$' << t->second;
                log.flush();
                if ((ttl_.count(t->first)) > 0 && (ttl_[t->first] > cur_time)) {
                    log << "3#" << t->first.length() << '$' << t->first << to_string(ttl_[t->first]).length() << '$' << ttl_[t->first];
                    log.flush();
                }
            }
            t++;
        }
        screen_file.close();
    }

    void load_screen(const string &name) {
        container_.clear();
        load_from_file(name);
        log.close();
        log.open("log.t");
        auto t = container_.begin();
        time_t cur_time = time(NULL);
        while (t != container_.end()) {
            if (t->first != "" || t->second != "") {
                log << "1#" << t->first.length() << '$' << t->first << t->second.length() << '$' << t->second;
                log.flush();
                if ((ttl_.count(t->first)) > 0 && (ttl_[t->first] > cur_time)) {
                    log << "3#" << t->first.length() << '$' << t->first << to_string(ttl_[t->first]).length() << '$' << ttl_[t->first];
                    log.flush();
                }
            }
            t++;

        }        
    }

    ~container() {
        log.close();
    }

    void set_val(const string &key, const string &value) {
        this->set_(key, value);
        log << "1#" << key.length() << '$' << key << value.length() << '$' << value;
        log.flush();
    }

    string get_val(const string &key) {
        if (ttl_.count(key) > 0 && ttl_[key] <= time(NULL))
            this->del_val(key);
        return container_[key];
    }

    void del_val(const string &key) {
        this->del_(key);
        log << "2#" << key.length() << '$' << key << "0$";
        log.flush();
    }

    int set_ttl(const string &key, const long long &ttl) {
        int res = this->set_ttl_(key, ttl + time(NULL));
        if (res > 0) {
            log << "3#" << key.length() << '$' << key << to_string(ttl + time(NULL)).length() << '$' << (ttl + time(NULL));
            log.flush();
        }
        return res;
    }

    int del_ttl() {
        auto it = ttl_set_.begin(), it_end = ttl_set_.end(), del = ttl_set_.begin();
        bool flag;
        time_t cur_time = time(NULL);
        while (it != it_end) {
            flag = false;
            if (it->second > cur_time) {
                flag = true;
                del = it;
            }
            it++;
            if (flag) {
                cout << "Deleted " << del->first << endl; 
                ttl_set_.erase(del);
            }
        }
    }
};