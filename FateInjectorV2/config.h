#include "pch.h"

#include <fstream>
#include <algorithm>

extern char working_dir[1024];
extern bool customProcName;
extern std::wstring delaystr;
extern std::wstring dllPath;
extern std::wstring procName;


class config {

protected:
    std::string path;
    //Config name and state
    std::wstring name;
    std::wstring value;

    // generated Config str
    std::wstring configstr;


public:
    config();

    bool useCDisk();

    bool loadConfig();

    bool saveConfig();

protected:

    int analyseInt();
    virtual bool analyseBool();
    virtual void analyseState();

    virtual std::wstring makeConfig();
};