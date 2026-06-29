#include <iostream>
#include <vector>
#include <algorithm>
#include "file.hpp"
using namespace std;

//custom datatype to store commands
typedef struct Command {
    string soundname;
    short int speed;
    short int volume;
} Command;

//checks if an alias table sub list contains an item
bool AliasExists(std::vector<string>& atlist, string element) {
    return (std::find(atlist.begin(), atlist.end(), element) != atlist.end());
}

void GetAliasTable (vector<string> &keywords, vector<string> &sounds, vector<string> &compiledkeywords, string atname) {
    //get alias table
    string aliastable;
    LoadFromFile(atname, aliastable);
    //parse alias table
    //into keywords, sounds and compiled keywords
    string prev = "";
    bool ifkeyword = false;
    for (char i : aliastable) {
        if (i == *"\n" || i == *" ") {
            //do nothing
        } else if (i == *"|") {
            compiledkeywords.push_back(prev);
            prev = "";
        } else if (i == *";") {
            if (ifkeyword) {
                keywords.push_back(prev);
            } else {
                sounds.push_back(prev);
            }
            ifkeyword = false;
            prev = "";
        } else {
            if (i == *"!") {
                ifkeyword = true;
            }
            prev = prev + i;
        }        
    }
}

int main() {
    const char escapechar = *";";
    const char seperatorchar = *",";

    const short int soundmaxspeed = 60;
    const short int soundminspeed = -60;
    const unsigned short int soundmaxvolume = 400;
    const unsigned short int soundminvolume = 0;
    //get alias table
    vector<string> keywords, sounds, compiledkeywords;
    GetAliasTable(keywords, sounds, compiledkeywords, "s3cfg.at");

    string input; getline(cin, input);
    //split file into separate commands
    vector<string> pos;
    string prev = "";
    for (char i : input) {
        if (i == *"\n" || i == *" ") {
            //do nothing
        } else if (i == escapechar) {
            pos.push_back(prev + seperatorchar);
            prev = "";
        } else {
            prev = prev + i;
        }        
    }

    //split commands into seperate tokens/arguments
    vector<Command> commands = {};
    for (string item : pos) {
        string prev = "";
        Command maincommand = {"", 0, 0};
        //index value, used for splitting the string
        unsigned char index = 1;
        for (char chr : item) {
            //parse the string
            if (chr == seperatorchar) {
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
    //output commands
    cout << "[";
    for (auto i : commands) {
        cout << " [" << i.soundname << "," << i.speed << "," << i.volume << "],";
    } cout << "]" << endl;
    unsigned int line = 0; 
    for (auto i : commands) {
        ++line;
        if (AliasExists(keywords, i.soundname)) {
            //do some check or smth
            //make sure it has the right arguments idk
            cout << "exists";
        } else if (AliasExists(sounds, i.soundname)) {
            //make sure speed and volume are within limits
            if (!(i.speed <= soundmaxspeed && i.speed >= soundminspeed)) {
                cerr << "Error with sound item '" << i.soundname << "' (at line " << line << "). Sound volume must be within " << soundmaxspeed << " units and " << soundminspeed << " units.";
            }
        } else {
            cerr << "Error with item '" << i.soundname << "': Item does not exist in alias table (as a keyword or a sound)";
        }
    }
}
