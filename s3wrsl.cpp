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

int main(int argc, char* argv[]) {
    const string aliastable = "s3cfg.at";

    const char escapechar = *";";
    const char seperatorchar = *",";
    const char commentchar = *"#";

    const short int soundmaxspeed = 60;
    const short int soundminspeed = -60;
    const unsigned short int soundmaxvolume = 400;
    const unsigned short int soundminvolume = 0;
    //first three values of criticals MUST be speed, volume and transpose
    /*const*/vector<string> criticals = {"!speed", "!volume", "!transpose", "!stop", "!jump", "!target"};
    const unsigned char critcount = 3;

    string file = "", input = "";
    if (argc > 2) {
        cerr << endl << "the $30 Web Compiler does not support multi-file compilation as of the moment.\n";
        cerr << "Please compile the files individually instead of all at once" << endl;
        return 2;
    } else {
        if (argc == 2) {
            file = argv[1];
            cout << argv[1];
        } else {
            cout << "\nInput s30web file: "; getline(cin, file);
        }
        cout << endl << "Loading file '" << file << "'...";
        LoadFromFile(file, input); 
        if (input == "") {
            cerr << endl << "File '" << file << "' was unable to be loaded. Make sure the file exists before retrying" << endl;
            return 2;
        } else if (GetFileType(file) != ".s30web") {
            cerr << endl << "File is of an invalid file format. Make sure the file is an '.s30web' file before retrying" << endl;
            return 2;
        }
    }

    //get alias table
    vector<string> keywords, functions, opcodes;
    cout << endl << "Retrieving alias table...";
    GetAliasTable(keywords, functions, opcodes, aliastable);
    //split file into separate commands
    vector<string> pos;
    string prev = "";
    bool comment = false;
    cout << endl << "Reading file...";
    for (char item : input) {
        //commenting system has errors
        if (item == commentchar) {
            comment = comment ? false : true;
        }
        if (comment || item == *"#" || item == *"\n" || item == *" " || item == *"\t") {
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
    //File wide errors
    if (comment == true) {
        //make sure there are no dangling comments
        cerr << "\n\nError found with a dangling (unenclosed) comment line" << endl;
        stopcompile = true;
    }
    //Specific command argument related errors
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
                    cerr << "\n\nError with keyword item '" << item.soundname << "': First argument must be either greater than or equal to zero" << endl; 
                    stopcompile = true;
                }
                for (unsigned char crit; crit <= critcount; ++crit) {
                    if ( !(item.volume >= -1 && item.volume <= 1)) {
                        cerr << "\n\nError with keyword item '" << item.soundname << "': Second argument must be within 1 and -1 (as those numbers map to it's button options)";
                        stopcompile = true;
                    }
                }
            } else {
                if (item.speed != 0 || item.volume != 0) {
                    cerr << "\nWarning with keyword item '" << item.soundname << "': This is a special keyword that takes no arguments. All arguments will be ignored" << endl;
                    //forcefully ignore arguments provided
                    item.speed = 0; item.volume = 0;
                }
            }
        } else if (AliasExists(functions, item.soundname)) {
            //make sure speed and volume are within limits
            if (!(item.speed <= soundmaxspeed && item.speed >= soundminspeed)) {
                stopcompile = true;
                cerr << "\n\nError with sound item '" << item.soundname << "' (at line " << line << "). Sound speed must be within " << soundmaxspeed << " units and " << soundminspeed << " units." << endl;
            }
            if (!(item.volume <= soundmaxvolume && item.volume >= soundminvolume)) {
                stopcompile = true;
                cerr << "\n\nError with sound item '" << item.soundname << "' (at line " << line << "). Sound volume must be within " << soundmaxvolume << " units and " << soundminvolume << " units." << endl;
            }
        } else {
            //if item is nonexistent in the alias table
            stopcompile = true;
            cerr << "\n\nError with item '" << item.soundname << "': Item does not exist in alias table (as a keyword or a sound)";
        }
    } if (stopcompile) { return 1; }
    cout << " No errors found!";

    //convert to operation codes and write it all to a buffer (pos)
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
                bool critcheck;
                for (unsigned char crit; crit <= critcount; ++crit) {
                    if (opcodes[commandID] == criticals[1]) {
                        critcheck = true;
                        break;
                    }
                }
                if (critcheck) {
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
