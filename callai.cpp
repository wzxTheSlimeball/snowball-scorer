#include "callai.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include "raylib.h"
#include <regex>
#include "safelog.h"
std::string unescapeStringRegex(const std::string& input) {
    std::string result=input;
    std::regex patterns[]={
        std::regex(R"(\\\\)"),
        std::regex(R"(\\n)"),
        std::regex(R"(\\r)"),
        std::regex(R"(\\t)"),
        std::regex(R"(\\")"),
        std::regex(R"(\\')"),
        std::regex(R"(\\b)"),
        std::regex(R"(\\f)"),
        std::regex(R"(\\v)"),
        std::regex(R"(\\0)")
    };
    std::string replacements[]={
        "\\","\n","\r","\t","\"","\'","\b","\f","\v","\0"
    };
    for(size_t i=0;i<sizeof(patterns)/sizeof(patterns[0]);++i) {
        result=std::regex_replace(result,patterns[i],replacements[i]);
    }
    std::regex unicodePattern(R"(\\u([0-9a-fA-F]{4}))");
    std::smatch match;
    while (std::regex_search(result, match, unicodePattern)){
        std::string hexStr=match[1].str();
        unsigned int codepoint;
        std::stringstream ss;
        ss<<std::hex<<hexStr;
        ss>>codepoint;
        std::string utf8Char;
        if(codepoint<=0x7F) {
            utf8Char=static_cast<char>(codepoint);
        }else if(codepoint <= 0x7FF){
            utf8Char+=static_cast<char>(0xC0 | (codepoint >> 6));
            utf8Char+=static_cast<char>(0x80 | (codepoint & 0x3F));
        }else if(codepoint <= 0xFFFF){
            utf8Char+=static_cast<char>(0xE0 | (codepoint >> 12));
            utf8Char+=static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            utf8Char+=static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        result.replace(match.position(),match.length(),utf8Char);
    }
    return result;
}
static size_t WriteCallback(void* contents,size_t size,size_t nmemb,std::string* output){
    size_t totalSize=size*nmemb;
    output->append((char*)contents,totalSize);
    return totalSize;
}
Response::Response(std::string shortContent,std::string contentFile){
    this->shortContent=shortContent;
    this->contentFile=contentFile;
}
Response::Response(){
    this->shortContent="";
    this->contentFile="";
}
Response::~Response(){
    this->shortContent="";
    this->contentFile="";
}
int Response::save(int mode,std::string content=""){
    if(mode==0){
        this->shortContent=unescapeStringRegex(content);
        return 0;
    }
    if(contentFile.empty()){
        safeLog(LOG_ERROR,"Response shortContent or contentFile is empty");
        return 0x01;
    }
    std::ofstream file(contentFile);
    if(!file.is_open()){
        safeLog(LOG_ERROR,"Response contentFile is not open");
        return 0x02;
    }
    file<<this->shortContent;
    file.close();
    return 0;
}
std::string Response::getShortContent(){
    return this->shortContent;
}
std::string Response::getContentFile(){
    return this->contentFile;
}
std::string parseOllamaResponse(const std::string& jsonResponse){
    if(jsonResponse.empty()){
        safeLog(LOG_ERROR,"jsonResponse is empty");
        return "";
    }
    if(jsonResponse.find("\"error\":\"")!=std::string::npos){
        safeLog(LOG_ERROR,"jsonResponse contains error, Error: %s",jsonResponse.c_str());
        return "";
    }
    std::string fullResponse;
    std::istringstream stream(jsonResponse);
    std::string line;
    int lineCount = 0;
    while(std::getline(stream,line)){
        lineCount++;
        size_t start=line.find("\"response\":\"")+12;
        size_t end=line.find("\"",start);
        std::string response=line.substr(start,end-start);
        safeLog(LOG_INFO, "Ollama response: %s", response.c_str());
        fullResponse+=response;
        if(line.find("\"done\":true")!=std::string::npos){
            break;
        }
    }
    return fullResponse;
}
std::string escapeJsonString(const std::string& input) {
    std::string output;
    for(char c:input){
        switch(c){
            case '"':output+="\\\"";break;
            case '\\':output+="\\\\\\";break;
            case '/':output+="\\/";break;
            case '\b':output+="\\b";break;
            case '\f':output+="\\f";break;
            case '\n':output+="\\n";break;
            case '\r':output+="\\r";break;
            case '\t':output+="\\t";break;
            case '&':output+="\\&";break;
            default:  output += c;break;
        }
    }
    return output;
}
Response callai(std::string model,std::string prompt,std::string contentFile,float temperature,float top_p,int max_tokens,int num_ctx,bool thinking,std::string stop_seq){
    Response response("",contentFile);
    std::string url="http://localhost:11434/api/generate";
    std::string postData="{\"model\":\""+escapeJsonString(model)+"\",\"prompt\":\""+escapeJsonString(prompt)+"\",\"options\":{"
    +"\"temperature\":"+std::to_string(temperature)+","
    +"\"top_p\":"+std::to_string(top_p)+","
    +"\"max_tokens\":"+std::to_string(max_tokens)+","
    +"\"num_ctx\":"+std::to_string(num_ctx)+","
    +"\"thinking\":"+std::string(thinking?"true":"false")+","
    +(stop_seq==""?"\"stop\":[\""+escapeJsonString(stop_seq)+"\"]":"")
    +"}}";
    //我转义的转义，不是转义我的转义，转义我的转义是转义转义转义的转义，我转义的转义，是转义转义转义转义的转义，所以我转义的转义，不是转义我的转义
    safeLog(LOG_INFO, "Ollama postData: %s", postData.c_str());
    CURL* curl=curl_easy_init();
    if(!curl){
        safeLog(LOG_ERROR,"curl_easy_init failed");
        return response;
    }
    curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
    curl_easy_setopt(curl,CURLOPT_POSTFIELDS,postData.c_str());
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback);
    std::string responseBody;
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&responseBody);
    CURLcode res=curl_easy_perform(curl);
    if(res!=CURLE_OK){
        safeLog(LOG_ERROR,"curl_easy_perform failed: %s",curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return response;
    }
    curl_easy_cleanup(curl);    
    safeLog(LOG_INFO, "Ollama origin response: %s", responseBody.c_str());
    response.shortContent=unescapeStringRegex(parseOllamaResponse(responseBody));
    contentFile.empty()?response.save(0,response.shortContent):response.save(1,response.shortContent);
    return response;
}

