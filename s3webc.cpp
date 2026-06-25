#include <iostream>
#include <vector>
using namespace std;

typedef struct Command {
    string soundname;
    short int speed;
    short int volume;
} Command;

int main() {
    char escapechar = ';';
    string input = "sma,1;   \n asm,2,-1;dih;";

    //split file into separate commmands
    vector<string> pos;
    string prev = "";
    for (char i : input) {
        if (i == *"\n" || i == *" ") {
            //do nothing
        } else if (i == escapechar) {
            pos.push_back(prev + ",");
            prev = "";
        } else {
            prev = prev + i;
        }        
    }
    cout << "[";
    for (auto i : pos) {
        cout << "(" << i << "), ";
    } cout << "]" << endl;

    //split commands into tokens
    vector<Command> commands = {};
    for (string item : pos) {
        string prev = "";
        Command maincommand = {"", 0, 0};
        //index value, used for splitting the string
        unsigned char index = 1;
        for (char chr : item) {
            //parse the string
            if (chr == *",") {
                if (index == 1){
                    maincommand.soundname = prev;
                } else if (index == 2) {
                    maincommand.speed = stoi(prev);
                } else if (index == 3) {
                    maincommand.volume = stoi(prev);
                }
                ++index;
                prev = "";
                
            } else {
                prev = prev + chr;
            }
        }
        commands.push_back(maincommand);
    }
    cout << "[";
    for (auto i : commands) {
        cout << " [" << i.soundname << "," << i.speed << "," << i.volume << "],";
    } cout << "]" << endl;
}
