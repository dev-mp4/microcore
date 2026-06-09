#ifndef ARGS_HPP
#define ARGS_HPP

#include <vector>

struct Parameters {
    unsigned long long memorySize;
    const char* firmwareFile;
    unsigned long long ticksCount;
    bool testMode;
    unsigned long long sigStart;
    unsigned long long sigEnd;
    const char* sigFile;
};

class ArgsParser {
public:
    ArgsParser(int argc, char** argv);
    ~ArgsParser();

    Parameters parse(Parameters defaults);

    static void printHelp();

private:
    std::vector<char*> args;
};

#endif // ARGS_HPP