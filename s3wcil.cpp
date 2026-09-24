#include <iostream>
#include <vector>
using namespace std;

int main() {
    //parse words
    string input = "List of Words\n";
    string temp = "";
    vector<string> pos;
    for(char i : input) {
        if (i == *" " || i == *"\n") {
            pos.push_back(temp);
            temp = "";
        } else {
            temp += i;
        }
    }
    for (string x : pos) {
        cout << x << endl;
    }
}
