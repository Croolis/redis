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

 public:
    container() {
        string path = "log";
        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1) {
            cout << "Can not open log\n";
        } else {
            char str[4096];
            int len = 0, p = -1, num = 0, condition = 0;
            string key, value;
            while (true) {
                p++;
                if (p == len) {
                    len = read(fd, str, sizeof(str));
                    if (len < 0)
                        cout << "Can not open log\n";
                    p = 0;
                }
                if (len <= 0)
                    break;
                if (condition == 0) {
                    container_[key] = value;
                    key = "";
                    value = "";
                }
                if (condition == 0 || condition == 2) {
                    if (str[p] == '$') {
                        condition = (condition + 1) % 4;
                        continue;
                    }
                    else {
                        num *= 10;
                        num += str[p] - '0';
                    }           
                }
                if (condition == 1) {
                    key += str[p];
                    num--;
                    if (num == 0) {
                        condition = (condition + 1) % 4;
                        continue;
                    }
                }
                if (condition == 3) {
                    value += str[p];
                    num--;
                    if (num == 0) {
                        condition = (condition + 1) % 4;
                        continue;
                    }
                }
            }
            close(fd);
        }
        log.open("log");
    }

    ~container() {
        log.close();
    }

    void set_val(string key, string value) {
        cout << "in container.set " << key << ' ' << value << endl;
        container_[key] = value;
        log << key.length() << '$' << key << value.length() << '$' << value;
        log.flush();
        ttl_.erase(key);
        auto it = ttl_set_.lower_bound(make_pair(key, 0));
        if (it != ttl_set_.end())
            it++;
        if (it != ttl_set_.end() && it->first == key)
            ttl_set_.erase(it);
    }

    string get(string key) {
        if (ttl_.count(key) > 0 && ttl_[key] <= time(NULL))
            this->del(key);
        return container_[key];
    }

    void del(string key) {
        container_.erase(key);
        ttl_.erase(key);
        auto it = ttl_set_.lower_bound(make_pair(key, 0));
        it++;
        if (it != ttl_set_.end() && it->first == key)
            ttl_set_.erase(it);
    }

    int set_ttl(string key, long long ttl) {
        if (container_.count(key) == 0)
            return 0;
        ttl_[key] = time(NULL) + ttl;
        ttl_set_.insert(make_pair(key, time(NULL) + ttl));
        return 1;
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