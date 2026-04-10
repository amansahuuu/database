#pragma once
#include <iostream>
// helpers use windows API; include minimal header
#include <Windows.h>

//#ifdef _DEBUG
#define LOG_MSG(x) std::cout << x << std::endl;
//#else
//#define LOG_MSG(x)
//#endif

#define LOG(x) std::cout << x << std::endl;

// LOG_ERROR prints to std::cerr with file/line/function info and colors the console output red briefly.
#define LOG_ERROR(x)                                                      \
    do {                                                                   \
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);      \
        std::cerr << __FILE__ << ":" << __LINE__ << " (" << __func__     \
                  << ") " <<"#>" << x << std::endl;                            \
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);      \
    } while (0);

#define LOG_FATAL_ERROR(x)                                      \
                                                     \
    do {                                                                   \
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);      \
        std::cerr << "\033[1mFATAL ERROR:\033[0m " << __FILE__ << ":" << __LINE__ << " (" << __func__     \
                  << ") " <<"#>" << x << std::endl;                            \
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);      \
    } while (0);
#define UNIVERSER_INCLUDE_STATEMENT #include "../types/types.h"

