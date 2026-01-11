#ifndef GADGETS_H
#define GADGETS_H
#include <iostream>
#include <vector>
#include <string>
#include <regex>

extern std::string scoreModel1,scoreModel2,scoreModelThinking,commentingModel;
extern std::string scoringPrompt;
extern bool formalPromptEdit;
extern float scoreTemperature;
extern float scoreTopP;
extern float commentingTemperature;
extern float commentingTopP;
void getConfig();
std::vector<std::string> split(std::string str,std::string delimiter);
int extractScoreNumberFromResponse(std::string str);
std::vector<int> extractAllNumbers(std::string str);

#endif