#include "args.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

ArgsParser::ArgsParser(int argc, char** argv) : args(argv, argv + argc) {}
ArgsParser::~ArgsParser() {}

Parameters ArgsParser::parse(Parameters defaults) {
    Parameters params = defaults;

    for (int i = 0; i < args.size(); i++) {
        std::string arg = std::string(args[i]);

        if (arg == "-m" || arg == "--memory") {
            if (i == args.size() - 1) {
                throw std::runtime_error("Value expected!");
            }

            std::string value = std::string(args[++i]);
            try {
                params.memorySize = std::stoull(value);
            } catch (const std::invalid_argument&) {
                throw std::runtime_error("Memory size argument is not an integer!");
            } catch (const std::out_of_range&) {
                throw std::runtime_error("Specified memory size is too large!");
            }
        } else if (arg == "-fw" || arg == "--firmware") {
            if (i == args.size() - 1) {
                throw std::runtime_error("Value expected!");
            }

            params.firmwareFile = args[++i];
        } else if (arg == "-t" || arg == "--ticks") {
            if (i == args.size() - 1) {
                throw std::runtime_error("Value expected!");
            }

            std::string value = std::string(args[++i]);
            try {
                params.ticksCount = std::stoull(value);
            } catch (const std::invalid_argument&) {
                throw std::runtime_error("Ticks count argument is not an integer!");
            } catch (const std::out_of_range&) {
                throw std::runtime_error("Specified ticks count is too large! Do not specify this parameter or use 0 to run endlessly.");
            }
        } else if (arg == "-tm" || arg == "--testmode") {
            params.testMode = true;
        } else if (arg == "-h" || arg == "--help") {
            printHelp();
            throw std::runtime_error("h"); // tell main function that we need to just display help and exit
        } else if (arg == "-ss" || arg == "--sigStart") {
            std::string value = std::string(args[++i]);
            try {
                params.sigStart = std::stoull(value, nullptr, 0);
            } catch (const std::invalid_argument&) {
                throw std::runtime_error("Singature start argument is not an integer!");
            } catch (const std::out_of_range&) {
                throw std::runtime_error("Singature start is too large!");
            }

            if (params.sigEnd != defaults.sigEnd && params.sigStart > params.sigEnd) {
                throw std::runtime_error("Signature start cannot be higher than signature end!");
            }
        } else if (arg == "-se" || arg == "--sigEnd") {
            std::string value = std::string(args[++i]);
            try {
                params.sigEnd = std::stoull(value, nullptr, 0);
            } catch (const std::invalid_argument&) {
                throw std::runtime_error("Singature end argument is not an integer!");
            } catch (const std::out_of_range&) {
                throw std::runtime_error("Singature end is too large!");
            }

            if (params.sigStart != defaults.sigStart && params.sigStart > params.sigEnd) {
                throw std::runtime_error("Signature start cannot be higher than signature end!");
            }
        } else if (arg == "-sf" || arg == "--sigFile") {
            if (i == args.size() - 1) {
                throw std::runtime_error("Value expected!");
            }

            params.sigFile = args[++i];
        }
    }

    return params;
}

void ArgsParser::printHelp() {
    std::cout << "\t-m  or --memory   <size>   - set memory size in bytes." << std::endl <<
                 "\t-fw or --firmware <file>   - set path to binary firmware file." << std::endl <<
                 "\t-t  or --ticks    <count>  - set count of ticks that CPU should do, do not specify or set to 0 to run endlessly." << std::endl <<
                 "\t-tm or --testmode          - used by test suite, disables output and prints registers values when exiting." << std::endl <<
                 "\t-h  or --help              - print this text." << std::endl <<
                 "\t-ss or --sigStart <value>  - set start address of memory dump" << std::endl <<
                 "\t-se or --sigEnd   <value>  - set end address of memory dump" << std::endl <<
                 "\t-sf or --sigFile  <file>   - set memory dump output filename" << std::endl;
}