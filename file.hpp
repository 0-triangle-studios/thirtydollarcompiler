#ifndef FILE_H
#define FILE_H
#include <iostream>
#include <fstream>

bool fileExists(std::string& inputfile);

std::string GetFileName(const std::string& filename, const std::string& filetype);

std::string GetFileType(std::string& filename);

void SaveToFile(std::string filename, std::string &input);

void LoadFromFile(std::string filename, std::string &output);

#endif