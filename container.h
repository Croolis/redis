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
        // TODO: No_key_in_container_exception
        return container_[key];
    }
};