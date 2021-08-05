// bin2h.cpp

#include "bin2h.h"

BinHeader::BinHeader(string outputLoc)
{
    if (outputLoc != "") OpenHeader(outputLoc);
    else isInit = false;
}

bool BinHeader::OpenHeader(string outputLoc)
{
    if (isInit) fHeader.close();
    fHeader.open(outputLoc.c_str(), ios::binary | ios::in);
    return (isInit = fHeader.is_open());
}

bool BinHeader::AddFile(string fileLoc, string arrName)
{
    if (isInit) {
        if (CheckArrName(arrName)) {
            ifstream fFile(fileLoc.c_str());
            if (fFile.is_open()) {
                int i = 0;
                char chr;
                fHeader << "const char " << arrName << "[]={";
                fFile.get(chr);
                if (fFile.good())
                    while (true) {
                        fHeader << CharHex(chr);
                        i++;
                        fFile.get(chr);
                        if (!fFile.good()) break;
                        else fHeader << ",";
                    }
                fHeader << "};\nconst long " << arrName << "_len=" << i << ";" << endl;
                binList[0].push_back(arrName);
                binList[1].push_back(fileLoc);
                return true;
            }
        }
    }
    return false;
}

bool BinHeader::CheckArrName(string arrName)
{
    return (CheckArrNameValid(arrName) && find(binList[0].begin(), binList[0].end(), arrName.c_str()) == binList[0].end() && find(binList[0].begin(), binList[0].end(), (arrName + "_len").c_str()) == binList[0].end());
}

void BinHeader::CloseHeader()
{
    if (isInit) {
        fHeader.close();
        isInit = false;
    }
}

BinHeader::~BinHeader()
{
    CloseHeader();
}

string CharHex(char chr)
{
    char tmp[5];
    snprintf(tmp, 5, "0x%02X, ", chr);
    return string(tmp);
}

bool CheckArrNameValid(string arrName)
{
    return (isalnum(arrName.at(0)) && arrName.substr(1).find_first_not_of(VALID_CHARS) == string::npos);
}

int main(int argc, char* argv[])
{
    BinHeader bh;
    string hfl, tmp;
    vector<string> vf[2];
    ostringstream stmp;
    do {
        cout << "Create header file at: ";
        getline(cin, hfl);
    } while (hfl == "");
    while (true) {
        cout << "Include file: ";
        getline(cin, tmp);
        if (!(tmp == "")) {
            vf[0].push_back(tmp);
            cout << "Array name: ";
            getline(cin, tmp);
            if (!bh.CheckArrName(tmp)) {
                cout << "That name is invalid or has been taken." << endl;
                vf[0].pop_back();
            }
            else vf[1].push_back(tmp);
        }
        cout << "Add another file? (y/N) ";
        getline(cin, tmp);
        if (tmp == "" || toupper(tmp.at(0)) == 'N') break;
    }
    cout << "==PROGRESS==\nCreating header file...";
    if (!bh.OpenHeader(hfl)) {
        cout << "\nERROR: Could not create header file." << endl;
        return 1;
    }
    for (int i = 0; i < vf[0].size(); i++) {
        cout << "\nAdding file '" << vf[0].at(i) << "' ...";
        stmp << "bin" << i;
        if (!bh.AddFile(vf[0].at(i), (vf[1].at(i) == "" ? stmp.str().c_str() : vf[1].at(i))))
            cout << "\nERROR: Could not read file '" << vf[0].at(i) << "'.";
        stmp.seekp(0);
    }
    cout << endl;
    return 0;
}
