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

    //!!so i don't exactly know what this is,
    //only that it's required
    bool operator==(const Command& other) const {
        return soundname == other.soundname && (speed == other.speed && volume == other.volume);
    }//apparently it allows the equal check to properly check if equal
} Command;

//checks if an alias table sub list contains an item
bool AliasExists(std::vector<string>& atlist, string element) {
    return (std::find(atlist.begin(), atlist.end(), element) != atlist.end());
}

void GetAliasTable (vector<string> &keywords, vector<string> &functions, vector<string> &compiledkeywords, string atname) {
    //get alias table
    string aliastable;
    LoadFromFile(atname, aliastable);
    //parse alias table
    //into keywords, functions and compiled keywords
    //keywords are technically a subset of functions
    string prev = "";
    bool ifkeyword = false;
    for (char item : aliastable) {
        if (item == *"\n" || item == *" ") {
            //do nothing
        } else if (item == *"|") {
            compiledkeywords.push_back(prev);
            prev = "";
        } else if (item == *";") {
            if (ifkeyword) {
                keywords.push_back(prev);
            }
            functions.push_back(prev);
            ifkeyword = false;
            prev = "";
        } else {
            if (item == *"!") {
                ifkeyword = true;
            }
            prev = prev + item;
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
    vector<string> keywords, functions, compiledkeywords;
    GetAliasTable(keywords, functions, compiledkeywords, "s3cfg.at");

    string input; getline(cin, input);
    //split file into separate commands
    vector<string> pos;
    string prev = "";
    for (char item : input) {
        if (item == *"\n" || item == *" ") {
            //do nothing
        } else if (item == escapechar) {
            pos.push_back(prev + seperatorchar);
            prev = "";
        } else {
            prev = prev + item;
        }        
    }

    //split commands into seperate tokens/arguments
    vector<Command> commands = {};
    for (string lineitem : pos) {
        string prev = "";
        Command maincommand = {"", 0, 0};
        //index value, used for splitting the string
        unsigned char index = 1;
        for (char chr : lineitem) {
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
    for (auto item : commands) {
        cout << " [" << item.soundname << "," << item.speed << "," << item.volume << "],";
    } cout << "]" << endl;
    
    //perform checks on all the values
    unsigned int line = 0;
    bool stopcompile = false; //flag used to stop further compilation
    for (auto item : commands) {
        ++line;
        if (AliasExists(keywords, item.soundname)) {
            //do some check or smth, specific to keywords
            //make sure it has the right arguments idk
        } else if (AliasExists(functions, item.soundname)) {
            //make sure speed and volume are within limits
            if (!(item.speed <= soundmaxspeed && item.speed >= soundminspeed)) {
                stopcompile = true;
                cerr << "Error with sound item '" << item.soundname << "' (at line " << line << "). Sound speed must be within " << soundmaxspeed << " units and " << soundminspeed << " units." << endl;
            }
            if (!(item.volume <= soundmaxvolume && item.volume >= soundminvolume)) {
                stopcompile = true;
                cerr << "Error with sound item '" << item.soundname << "' (at line " << line << "). Sound volume must be within " << soundmaxvolume << " units and " << soundminvolume << " units." << endl;
            }
        } else {
            //if item is nonexistent in the alias table
            stopcompile = true;
            cerr << "Error with item '" << item.soundname << "': Item does not exist in alias table (as a keyword or a sound)";
        }
    } if (stopcompile) { return -1; }

    //convert to compiled keywords and write it all to a file
    for (auto item : commands) {
        //link command to it's compiled command
        auto iterator = std::find(functions.begin(), functions.end(), item.soundname);
    //there was an if statement here (from AI) that i felt was redundant 
    //  if(iterator != functions.end()) { :the code below: } else {cerr << "System error, please restart the program";}
        unsigned short int commandID = std::distance(functions.begin(), iterator);
        cout << "First command ID found: " << commandID << " " << functions[commandID] << endl;
    }
}
