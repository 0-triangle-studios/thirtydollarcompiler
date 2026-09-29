#include <iostream>
#include <vector>
using namespace std;

//small function to get the index of str vector item
inline unsigned int StrGetIndex(vector<string>& list, string& item) {
    for (unsigned int i = 0; i < (list.end() - list.begin()); ++i) {
        if (list[i] == item) { return i; }
    }
    std::cout << "Not found" << std::endl;
    //if not found, return what should be an invalid index
    return (2 + list.end() - list.begin());
} 
//main functions for evaluating variables
string SolveAllVars(string input, vector<string> &var_name, vector<int> &var_value) {
    //parse words
    string temp = "";
    string pos;
    bool isvar = false;
    for(char i : input) {
        if (i == *" ") {
            //do nothing
        } else if (i == *"&") {
            isvar = true;
        } else if (i == *",") {
            if (isvar) {
                //get corresponding var value assigned to it's var name
                const unsigned int varlc = StrGetIndex(var_name, temp);
                pos += to_string(var_value[varlc]) + ",";
                isvar = false;
            } else {
                pos += temp + ",";
            }
            temp = "";
        } else {
            temp += i;
        }
    }
    pos += temp;
    return pos;
}

int main() {
    vector<string> var_name = {"vx", "vy", "vz"};
    vector<int> var_value = {10, 5, 2};
    string input = "a, 2, 5";
    cout << SolveAllVars(input, var_name, var_value) << endl;
}
