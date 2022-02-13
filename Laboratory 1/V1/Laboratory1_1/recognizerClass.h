#pragma once

#ifndef _H_THECONTEXT
#define _H_THECONTEXT
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include "recognizerClass_sm.h"
#include <vector>
#include <map>
#include <limits>
#include <utility>

using namespace std;

class recognizerClass
{
private:
    recognizerClassContext _fsm;

    bool isAcceptable;
    int partLen;  // to control the length of a part of a string
    std::string buf; // for statistics
    std::map<std::string, int> table;

public:
    recognizerClass();
    ~recognizerClass() {};

    // main function
    bool CheckString(const char* s);
    bool reset();

    // setters
    inline void Acceptable() {
        isAcceptable = true;
    };

    inline void Unacceptable() {
        isAcceptable = false;
    };

    // condition check
    bool serverNameLen();
    bool numberOfSlashes();
    bool directoryNameLen();
    bool fileNameLen();

    // to work with a buffer
    void addSymb(char symb);
    void addToTable();


    // output functions
    bool getVal() {
        return isAcceptable;
    }
    std::map<std::string, int> getStat();
};

#endif