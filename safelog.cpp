#include "safelog.h"
#include <iostream>
#include <mutex>
#include <stdarg.h>
#include <chrono>
#include "raylib.h"

std::mutex logMutex;
void safeLog(int logLevel,const char* format,...)
{
    std::lock_guard<std::mutex> lock(logMutex);
    auto logTime=std::chrono::system_clock::now();
    auto logTimeT=std::chrono::system_clock::to_time_t(logTime);
    std::string logTimeStr=std::ctime(&logTimeT);
    logTimeStr.pop_back();
    logTimeStr="["+logTimeStr+"]";
    std::string logHeader="";
    switch(logLevel){
        case LOG_INFO:
            logHeader="[INFO]";
            break;
        case LOG_ERROR:
            logHeader="[ERROR]";
            break;
        case LOG_WARNING:
            logHeader="[WARNING]";
            break;
        default:
            logHeader="[UNKNOWN_LEVEL]";
            break;
    }
    va_list args;
    std::string logFormat=std::string(format);
    logFormat=" "+logFormat+"\n";
    va_start(args,format);
    vfprintf(stdout,(logTimeStr+" "+logHeader+" "+logFormat).c_str(),args);
    va_end(args);
    fflush(stdout);
}
