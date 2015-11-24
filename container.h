#include <string>
#include <map>

using namespace std;

class container {
 private:
    map<string, string> container_;

 public:
    container() {}

    void set(string key, string value) {
        container_[key] = value;
    }

    string get(string key) {
        return container_[key];
    }
};