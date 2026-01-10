#ifndef CALLAI_H
#define CALLAI_H
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <iostream>
#include <string>
#include <curl/curl.h>
#include "raylib.h"
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
class Response{
    public:
    std::string shortContent;
    std::string contentFile;
    int save(int mode,std::string content);
    std::string getShortContent();
    std::string getContentFile();
    Response(std::string shortContent,std::string contentFile);
    Response();
    ~Response();
};
std::string escapeJsonString(const std::string& input);
Response callai(std::string model,std::string prompt,std::string contentFile,float temperature,float top_p,int max_tokens,int num_ctx,bool thinking);
#endif