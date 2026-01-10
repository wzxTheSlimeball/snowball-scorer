#ifndef SAFELOG_H
#define SAFELOG_H

#include <iostream>
#include <mutex>
#include <stdarg.h>
#include "raylib.h"
#include <chrono>
void safeLog(int logLevel,const char* format,...);

#endif