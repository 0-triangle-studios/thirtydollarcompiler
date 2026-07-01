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

void GetAliasTable (vector<string> &keywords, vector<string> &functions, vector<string> &opcodes, string atname) {
    //get alias table
    string aliastable;
    LoadFromFile(atname, aliastable);
    //parse alias table
    //into keywords, functions and operation codes
    //keywords are technically a subset of functions
    string prev = "";
    bool ifkeyword = false;
    for (char item : aliastable) {
        if (item == *"\n" || item == *" ") {
            //do nothing
        } else if (item == *"|") {
            opcodes.push_back(prev);
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
    //first two values of criticals MUST be speed and volume
    /*const*/vector<string> criticals = {"!speed", "!volume", "!stop", "!jump", "!target"};

    string file, input; 
    cout << "\nInput s30web file: "; getline(cin, file);
    cout << endl << "Loading file...";
    LoadFromFile(file, input); 

    //get alias table
    vector<string> keywords, functions, opcodes;
    cout << endl << "Retrieving alias table...";
    GetAliasTable(keywords, functions, opcodes, "s3cfg.at");
    //split file into separate commands
    vector<string> pos;
    string prev = "";
    cout << endl << "Reading file...";
    for (char item : input) {
        if (item == *"\n" || item == *" " || item == *"\t") {
            //do nothing
        } else if (item == escapechar) {
            pos.push_back(prev + seperatorchar);
            prev = "";
        } else {
            prev = prev + item;
        }        
    }//for (string posstr : pos) { cout << "[" << posstr << "], "; }

    //split commands into seperate tokens/arguments
    vector<Command> commands = {};
    cout << endl << "Parsing file...";
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
    
    //perform checks on all the values
    unsigned int line = 0;
    bool stopcompile = false; //flag used to stop further compilation
    cout << endl << "Checking file for errors:";
    for (auto item : commands) {
        ++line;
        if (AliasExists(keywords, item.soundname)) {
            //do the actual checks for keywords
            //get command index
            unsigned short int commandID = std::distance(functions.begin(), std::find(functions.begin(), functions.end(), item.soundname));
            //check if keyword maps to a critical compiled keyword
            if (AliasExists(criticals, opcodes[commandID])) {
                //make sure speed is greater than 0
                if (item.speed < 0) {
                    cerr << "Error with keyword item '" << item.soundname << "': First argument must be either greater than or equal to zero" << endl; 
                    stopcompile = true;
                }
                if ((opcodes[commandID] == criticals[1] || opcodes[commandID] == criticals[0])) {
                    if ( !(item.volume >= -1 && item.volume <= 1)) {
                        cerr << "Error with keyword item '" << item.soundname << "': Second argument must be within 1 and -1 (as those numbers map to it's button options)";
                        stopcompile = true;
                    }
                }
            } else {
                if (item.speed != 0 || item.volume != 0) {
                    cerr << "Warning with keyword item '" << item.soundname << "': This is a special keyword that takes no arguments. All arguments will be ignored" << endl;
                    //forcefully ignore arguments provided
                    item.speed = 0; item.volume = 0;
                }
            }
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
    cout << " No errors found!";

    //convert to operation codes and write it all to a file
    pos = {};
    cout << endl << "Mapping commands to thirtydollar.website operation codes";
    for (auto item : commands) {
        //link command to it's compiled command
        unsigned short int commandID = std::distance(functions.begin(), std::find(functions.begin(), functions.end(), item.soundname));
                
        //format item into var@x@y (for keyword) and var@x%y (for function) syntax
        string opcode = opcodes[commandID];
        if (item.speed != 0 || AliasExists(keywords, item.soundname)) {
            opcode += "@" + to_string(item.speed);
        }
        if (item.volume != 0){
            if (!AliasExists(keywords, item.soundname)) { 
                opcode += "%" + to_string(item.volume);
            } else {
                if (opcodes[commandID] == criticals[1] || opcodes[commandID] == criticals[0]) {
                    //setspeed and setvolume has a custom character instead of a number
                    if (item.volume == 1) {
                        opcode += "@x";
                    } else if (item.volume == -1) {
                        opcode += "@+";
                    }
                } else {
                    opcode += "@" + to_string(item.volume);
                }
            }
            
        }
        pos.push_back(opcode);
    }
    //write compiled commands
    cout << endl << "Writing to file...";
    string output = "";
    for (auto item : pos) {
        output += item + "|";
    }
    if (output == "") {
        cout << "\n\nFile was not able to be written. Fix any errors before recompiling, or make sure your file is not corrupted." << endl;
    } else {
    string outputname = GetFileName(file, "moai");
    SaveToFile(outputname, output);
    cout << "\n\nAll done! You can see the compiled .moai file in your current directory, under the name of '" << outputname << "'!" << endl;
    return 0;
    }
}
