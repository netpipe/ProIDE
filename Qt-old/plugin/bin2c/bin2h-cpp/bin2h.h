// bin2h.h
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

class BinHeader {
    ofstream fHeader;
    bool isInit;
    vector<string> binList[2];
public:
    BinHeader(string outputLoc = "");
    bool OpenHeader(string outputLoc);
    bool AddFile(string fileLoc, string arrName = "");
    bool CheckArrName(string arrName);
    void CloseHeader();
    ~BinHeader();
};

string CharHex(char chr);

bool CheckArrNameValid(string arrName);
#define VALID_CHARS        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_" 
