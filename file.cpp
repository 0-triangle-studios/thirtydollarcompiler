#include <iostream>
#include <fstream>
#include "file.hpp"

bool fileExists(std::string& inputfile) {
    //opens selected file, returns if it was successful, 
    std::ifstream file(inputfile);
    bool check = file.good();
    //closes file afterwards
    file.close();
    return check; 
}

std::string GetFileName(const std::string& filename, const std::string& filetype) {
    //searches filesystem directory for an unused file name
    std::string file = filename + '.' + filetype;
    while (fileExists(file)) {
        file += '.' + filetype;
    }
    return file;
}

//this function needs fixing
std::string GetFileType(std::string& filename) {
    std::string filetype = "";
    bool foundfiletype = false;
    for(auto letter: filename) {
        //std::cout << letter << filename;
        if (letter == *".") {
            if (foundfiletype) {
                filetype = "";
            } else {
                foundfiletype = true;
            }
        }
        if (foundfiletype) {
            filetype += letter;
        }
    }
    return filetype;
}

void SaveToFile(std::string filename, std::string &input) {
    std::ofstream mainfile(filename);
    //save to file if file is fit for use
    if (mainfile.good() && mainfile.is_open()) {
        mainfile << input;
        mainfile.close();
    } else {
        std::cout << "Unable to open file";
    }
}

void LoadFromFile(std::string filename, std::string &output) {
    std::ifstream mainfile(filename);
    //load from file if file is fit for use
    if (mainfile.good() && mainfile.is_open()) {
        std::string line;
        while (std::getline(mainfile, line)) {
            output += line + "\n";
        }
        mainfile.close();
    } else {
        std::cout << "Unable to open file";
    }
}

/*int main() {
    std::string stream = ""; 
    stream += "Hello, World!\n"; stream += "This is a C++ file writing example.\n";
    //write data to file
    SaveToFile(GetFileName("ext", "conf"), stream);
    return 0;
}*/   
