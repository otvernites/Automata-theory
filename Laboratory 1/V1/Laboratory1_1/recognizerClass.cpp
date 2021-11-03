#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif

#define _CRT_SECURE_NO_WARNINGS
#include "recognizerClass.h"
using namespace std;

recognizerClass::recognizerClass() : _fsm(*this) {
    isAcceptable = false;
    partLen = 0;
}

bool recognizerClass::reset() {
    _fsm.Unknown();
    _fsm.setState(Map1::Start);
    return isAcceptable;
}

bool recognizerClass::CheckString(const char* s) {
    _fsm.enterStartState();

    if ((*s == ' ') || (*s == '\t')) {
        _fsm.Whitespace();
        ++s;
    }

    for (int i = 0; i < 6; ++i) {
        if (((*s >= 'a') && (*s <= 'z')) || ((*s >= 'A') && (*s <= 'Z')) || (*s == ':')) {
            _fsm.Letter(*s);
        }
        else if ((*s == '/') && (*(s - 1) == '/')) {
            _fsm.Slash(*s);
            this->addSymb(*(s-1));
        }
        ++s;
    }

    std::string sample = "nfs://";
    if (buf != sample) {
        buf.clear();
        reset();
        return isAcceptable;
    }

    while (*s != '\0') {
        if (((*s >= 'a') && (*s <= 'z')) || ((*s >= 'A') && (*s <= 'Z'))) {
            ++partLen;
            _fsm.Letter(*s);

        }
        else if (*s == '/') {
            if (*(s + 1) == '\0') {
                _fsm.Unknown();
                return isAcceptable;
            }
            else if (*(s - 1) != '/') {  // 2 forward slashes
                partLen = 0;
                _fsm.Slash(*s);
            }
            else {
                _fsm.Unknown();
                return isAcceptable;
            }
        }
        else if (((*s == '.') || (*s == '_')) &&
                     ((string)_fsm.getState().getName() != (string)"Map1::SName_state")) {
            ++partLen;
            cout << _fsm.getState().getName() << endl;
            _fsm.PointOrUnderline(*s);
        }
        else if (*s == '\n') {
            _fsm.EOS();
            return isAcceptable;
        }
        else {
            _fsm.Unknown();
            return isAcceptable;
        }
        ++s;
    }

    _fsm.EOS();
    _fsm.setState(Map1::Start);
    return isAcceptable;
}

bool recognizerClass::serverNameLen() { 
    int maxSize = 15;
    if (this->partLen < maxSize) {
        return true;
    }
    return false;
}

bool recognizerClass::numberOfSlashes() {
    int maxSize = 1;
    if (this->partLen < maxSize) {
        return true;
    }
    return false;
}

bool recognizerClass::directoryNameLen() { 
    int maxSize = 20;
    if (this->partLen < maxSize) {
        return true;
    }
    return false;
}

bool recognizerClass::fileNameLen() { 
    int maxSize = 12;
    if (this->partLen < maxSize) {
        return true;
    }
    return false;
}

void recognizerClass::addSymb(char symb) {
    buf.push_back(symb);
}

void recognizerClass::addToTable() {
    int i = buf.size();
    int j = i;
    while ((i > 0) && (buf[i] != '/')) {
        --i;
    }
    ++i;
    std::string fName;
    for (i; i < j; ++i) {
        fName.push_back(buf[i]);
        
    }
    buf.clear();
    int number;
    if (table.contains(fName)) {
        ++table[fName];
    }
    else {
        table.insert(pair<string, int>(fName, 1));
    }
}

     std::map<std::string, int> recognizerClass::getStat() {
        return this->table;
    }


