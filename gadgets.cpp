#include "gadgets.h"
#include "safelog.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

std::string scoreModel1,scoreModel2,scoreModelThinking,commentingModel;
std::string scoringPrompt;
bool formalPromptEdit=false;
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
    std::regex scoringPromptPattern(R"(\"scoringPrompt\"\s*:\s*\"([^\"\\]+)\")");
    if(std::regex_search(config,match,scoringPromptPattern)){
        scoringPrompt=match[1];
    }
    std::regex formalPromptEditPattern(R"(\"formalPromptEdit\"\s*:\s*(true|false))");
    if(std::regex_search(config,match,formalPromptEditPattern)){
        formalPromptEdit=(match[1]=="true");
    }
    if(formalPromptEdit){
        std::regex formalBackgroundPattern(R"(\"background\"\s*:\s*\"([^\"\\]+)\")");
        std::regex formalWritingPattern(R"(\"writing\"\s*:\s*\"([^\"\\]+)\")");
        std::regex formalCommentingStandardPattern(R"(\"commenting standard\"\s*:\s*\"([^\"\\]+)\")");
        std::string background,writing,commentingStandard;
        if(std::regex_search(config,match,formalBackgroundPattern)){
            background=match[1];
        }
        if(std::regex_search(config,match,formalWritingPattern)){
            writing=match[1];
        }
        if(std::regex_search(config,match,formalCommentingStandardPattern)){
            commentingStandard=match[1];
        }
        scoringPrompt=background+writing+commentingStandard;
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
    std::smatch m;
    std::regex r_frac(R"((\d{1,3})\s*/\s*60)");
    if(std::regex_search(str,m,r_frac) && m.size()>=2){
        try{
            return std::stoi(m[1].str());
        }catch(...){ }
    }
    std::regex r_total(R"((?:Total\s*Score\s*[:：]?|Total\s*Score)\s*(\d{1,3}))", std::regex::icase);
    if(std::regex_search(str,m,r_total) && m.size()>=2){
        try{
            return std::stoi(m[1].str());
        }catch(...){ }
    }
    std::vector<int> numbers=extractAllNumbers(str);
    if(numbers.empty()){
        return -1;
    }
    for(auto it = numbers.rbegin(); it!=numbers.rend(); ++it){
        if(*it>=0 && *it<=60) return *it;
    }
    return numbers.back();
}