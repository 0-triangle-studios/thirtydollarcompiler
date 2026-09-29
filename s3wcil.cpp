#include <iostream>
#include <vector>
using namespace std;

//s3ml function data structure
typedef struct Function {
    string name;
    vector<string> args;
    vector<string> contents;
} Function;

//small function to get the index of str vector item
inline unsigned int StrGetIndex(vector<string>& list, string& item) {
    for (unsigned int i = 0; i < (list.end() - list.begin()); ++i) {
        if (list[i] == item) { return i; }
    }
    std::cout << "Not found" << std::endl;
    //if not found, return what should be an invalid index
    return (2 + list.end() - list.begin());
} 
//main function for evaluating variables
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
    string input = "&var = 4; @func {\na, &var, 3;\nb;\nc;\n} @main {\nc;@func, &var, 5;\n}";
    //parse words into data (and command later)
    string temp = "";
    //Split the strings into variables, functions, and their values and commands
    bool isvar = 0, isfunc = 0;
    vector<string> var_name;
    vector<int> var_value;
    vector<string> func_header;
    vector<string> *func_command = new vector<string>;
    //xi stands for indeX Input, and also this is an infinite loop
    for (unsigned int xi = 0;; ++xi) {
        //parse text
        if (input[xi] == *"\0") {
            //exit the loop
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
                    (*func_command).push_back(temp);
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
        //check if it is a function
        if (input[xi] == *"@") {
            isfunc = true;
        }
        //check if it is a non function variable
        if (input[xi] == *"&" && !isfunc) {
            
            isvar = true;
        } 
    }
    for (unsigned char d = 0; d < (func_header.end() - func_header.begin()); ++d) {
        cout << "Function Name: " << func_header[d] << "\n\tContents: " <<  (*func_command)[d] << endl;
    }
    for (unsigned char d = 0; d < (var_name.end() - var_name.begin()); ++d) {
        cout << "Variable Name: " << var_name[d] << "\n\tValue: " << var_value[d] << endl;
    }

    //Parse function names and contents
    vector<Function> *functions = new vector<Function>;
    Function tempfn = {"", {}, {}}; 
    for (unsigned int nm = 0; nm < (func_header.end() - func_header.begin()); ++nm) {
        bool isname = true;
        string temp = "";
        //parse function header
        for(char ch : func_header[nm]) {
            if (ch == *"&") {
                if (isname) {
                    //put name into tempfn.name
                    tempfn.name = temp;
                    isname = false;
                } else {
                    //put arguments into tempfn.args
                    tempfn.args.push_back(temp);
                }
                //reset temp
                temp = "";
            } else {
                //continue adding to temp
                temp += ch;
                //cout << temp << endl;
            }
        } //push back final temp
        if (isname) {
            tempfn.name = temp;
        } else {
            tempfn.args.push_back(temp);
        }

        temp = "";
        //parse function commands
        for (char ch : (*func_command)[nm]) {
            if (ch == *";") {
                //put command into tempfn.contents
                tempfn.contents.push_back(temp);
                //reset temp
                temp = "";
            } else {
                //continue adding to temp
                temp += ch;
            }
        }
        //save tempfn to functions
        (*functions).push_back(tempfn);
        //reset tempfn
        tempfn = {"", {}, {}}; 
    }

    //Evaluate function content
    for(Function fn : (*functions)) {
        //output func headers, for debugging
        cout << "Func name:" << fn.name << endl;
        for (string x : fn.args) {
            cout << "    Func arg:" << x << endl;
        }
        for (string x : fn.contents) {
            cout << "\tFunc command:" << SolveAllVars(x, var_name, var_value) << endl;
        }
    }
    //clean up memory
    delete func_command;
    delete functions;
}
