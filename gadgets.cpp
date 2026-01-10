#include "gadgets.h"
#include "safelog.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

std::string scoreModel1,scoreModel2,scoreModelThinking,commentingModel;
void getConfig()
{
    std::ifstream configFile("./config.json");
    if(!configFile.is_open()){
        safeLog(LOG_ERROR,"Failed to open config.json");
        return;
    }
    std::string config="";
    std::string line;
    while(std::getline(configFile,line)){
        config+=line+"\n";
    }
    configFile.close();
    std::regex scoreModel1Pattern(R"(\"scoreModel1\"\s*:\s*\"([^\"\\]+)\")");
    std::regex scoreModel2Pattern(R"(\"scoreModel2\"\s*:\s*\"([^\"\\]+)\")");
    std::regex scoreModelThinkingPattern(R"(\"scoreModelThinking\"\s*:\s*\"([^\"\\]+)\")");
    std::regex commentingModelPattern(R"(\"commentingModel\"\s*:\s*\"([^\"\\]+)\")");
    std::smatch match;
    if(std::regex_search(config,match,scoreModel1Pattern)){
        scoreModel1=match[1];
    }
    if(std::regex_search(config,match,scoreModel2Pattern)){
        scoreModel2=match[1];
    }
    if(std::regex_search(config,match,scoreModelThinkingPattern)){
        scoreModelThinking=match[1];
    }
    if(std::regex_search(config,match,commentingModelPattern)){
        commentingModel=match[1];
    }
}
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
int extractScoreNumberFromResponse(std::string str){
    int idx=str.find("/60");
    if(idx==std::string::npos){
        std::vector<int> numbers=extractAllNumbers(str);
        if(numbers.empty()){
            return -1;
        }
        if(numbers[0]<=10||numbers[0]>=60){
            return numbers[numbers.size()-1];
        }
        return numbers[0];
    }
    if(idx==std::string::npos){
        return -1;
    }
    std::string num=str.substr(idx-1,2);
    return std::stoi(num);
}