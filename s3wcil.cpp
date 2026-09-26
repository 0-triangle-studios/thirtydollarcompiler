#include <iostream>
#include <vector>
using namespace std;

int main() {
    string input = "@func {\na, 2, 3;\nb;\nc;\n} &var = 4;";
    //parse words into data (and command later)
    string temp = "";
    bool isvar, isfunc;
    vector<string> var_name;
    vector<int> var_value;
    vector<string> func_header;
    vector<string> func_command;
    //xi stands for indeX Input
    for (unsigned int xi = 0; xi >= 0; ++xi) {
        //parse text
        if (input[xi] == *"\0") {
            //reached end of string
            break;
        } else if (input[xi] == *" " || input[xi] == *"\n" || input[xi] == *"\t") {
            //do nothing
        } else {
            if (isfunc) {
                if (input[xi] == *"{") {
                    //store func header, next letter will be collected as commands
                    func_header.push_back(temp);
                    temp = "";
                } else if (input[xi] == *"}") {
                    //store all commands
                    func_command.push_back(temp);
                    //reset all
                    temp = "";
                    isfunc = false;
                } else {
                    temp += input[xi];
                }
            }
            if (!isfunc && isvar){
                if (input[xi] == *"=") {
                    //store varname, next letter will be collected as numbers
                    var_name.push_back(temp);
                    temp = "";
                } else if (input[xi] == *";") {
                    //store number (do error handling later)
                    var_value.push_back(stoi(temp));
                    //reset all
                    temp = "";
                    isvar = false;
                } else {
                    //continue getting letters
                    temp += input[xi];
                }
            }
        }

        //check for variable characters (& and %)
        if (input[xi] == *"@") {
            isfunc = true;
        }
        if (input[xi] == *"&") {
            //make sure it is a variable
            isvar = true;
        } 
    }
    for (unsigned char d = 0; d < (func_header.end() - func_header.begin()); ++d) {
        cout << "Function Name: " << func_header[d] << "\nValue: " << func_command[d] << endl;
    }
    for (unsigned char d = 0; d < (var_name.end() - var_name.begin()); ++d) {
        cout << "Variable Name: " << var_name[d] << "\nValue: " << var_value[d] << endl;
    }
}
