#include <iostream>
#include <vector>
using namespace std;

int main() {
    string input = "&var = 4; &var2 = 6;";
    //parse words into data (and command later)
    string temp = "";
    bool isvar;
    vector<string> var_name;
    vector<int> var_value;
    for(const char i : input) {
        //parse text
        if (i == *" " || i == *"\n" || i == *"\t") {
            //do nothing
        } else if (isvar){
            if (i == *"=") {
                //store varname, next letter will be collected as numbers
                var_name.push_back(temp);
                temp = "";
            } else if (i == *";") {
                //store number (do error handling later)
                var_value.push_back(stoi(temp));
                //reset all
                temp = "";
                isvar = false;
            } else {
                //continue getting letters
                temp += i;
            }
        }

        //check for variable characters (& and %)
        if (i == *"&") {
            //make sure it is a variable
            isvar = true;
        } 
    }
    for (unsigned char d = 0; d < 2; ++d) {
        cout << "Name: " << var_name[d] << "\nValue: " << var_value[d] << endl;
    }
}
