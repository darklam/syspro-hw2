//
// Created by John Lamis on 2019-04-13.
//

#include "Logger.hpp"
#include "Arguments.hpp"
#include <cstdlib>
#include <fstream>

Logger* Logger::instance = NULL;

Logger* Logger::getInstance() {
    if (instance == NULL) {
        instance = new Logger;
    }
    return instance;
}

void Logger::log(char *message) {
    std::fstream file;
    file.open(Arguments::getInstance()->getLogFile(), std::fstream::app);

    file << message << "\n";
    file.close();
}
