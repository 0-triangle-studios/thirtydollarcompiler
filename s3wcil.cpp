#include <iostream>
#include <vector>
using namespace std;

int main() {
    //parse words
    string input = "List of Words";
    string temp = "";
    char stopchr = *" ";
    vector<string> pos;
    for(const char i : input) {
        if (i == stopchr) {
            pos.push_back(temp);
            temp = "";
        } else {
            temp += i;
        }
    }
    pos.push_back(temp);
    for (string x : pos) {
        cout << x << endl;
    }
}
