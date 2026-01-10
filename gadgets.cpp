#include "gadgets.h"
#include <iostream>
#include <vector>
#include <string>
#include <regex>

std::vector<std::string> split(std::string str,std::string delimiter){
    std::vector<std::string> paragraphs;
    size_t pos=0;
    std::string token;
    while((pos=str.find(delimiter))!=std::string::npos){
        token=str.substr(0,pos);
        paragraphs.push_back(token);
        str.erase(0,pos+delimiter.length());
    }
    paragraphs.push_back(str);
    return paragraphs;
}
std::vector<int> extractAllNumbers(std::string str){
    std::vector<int> numbers;
    std::regex numberPattern(R"(\d+)");
    std::sregex_iterator iter(str.begin(),str.end(),numberPattern);
    std::sregex_iterator end;
    while(iter!=end){
        std::string number=iter->str();
        numbers.push_back(std::stoi(number));
        iter++;
    }
    return numbers;
}
std::string extractFirstNumber(std::string str){
    int idx=str.find("/60");
    if(idx==std::string::npos){
        std::vector<int> numbers=extractAllNumbers(str);
        if(numbers.empty()){
            return "";
        }
        return std::to_string(numbers[0]);
    }
    if(idx==std::string::npos){
        return str;
    }
    std::string num=str.substr(idx-1,2);
    return num;
}