#ifndef GADGETS_H
#define GADGETS_H
#include <iostream>
#include <vector>
#include <string>
#include <regex>

std::vector<std::string> split(std::string str,std::string delimiter);
std::string extractFirstNumber(std::string str);
std::vector<int> extractAllNumbers(std::string str);

#endif