//
// Created by John Lamis on 2019-03-25.
//

#include "Arguments.hpp"
#include <cstdlib>
#include <cstring>

Arguments* Arguments::instance = NULL;

void Arguments::initialize(int count, char** args) {
    int i = 0;
    while (i < count) {
        char* current = args[i++];
        if (strcmp(current, "-n") == 0) {
            this->id = atoi(args[i++]);
        }
        if (strcmp(current, "-c") == 0) {
            this->commonDir = args[i++];
        }
        if (strcmp(current, "-i") == 0) {
            this->inputDir = args[i++];
        }
        if (strcmp(current, "-b") == 0) {
            this->bufferSize = atoi(args[i++]);
        }
        if (strcmp(current, "-m") == 0) {
            this->mirrorDir = args[i++];
        }
        if (strcmp(current, "-l") == 0) {
            this->logFile = args[i++];
        }
    }
}

Arguments* Arguments::getInstance() {
    if (instance == NULL) {
        instance = new Arguments;
    }
    return instance;
}

Arguments::Arguments() {
    this->mirrorDir = "/Users/johnlamis/Desktop/ergasia/mirror";
    this->inputDir = "/Users/johnlamis/Desktop/ergasia/input";
    this->commonDir = "/Users/johnlamis/Desktop/ergasia/common";
    this->logFile = "/Users/johnlamis/Desktop/ergasia/logs.txt";
    this->bufferSize = 1000000;
    this->id = 69;
}

char* Arguments::getCommonDir() {
    return commonDir;
}

char* Arguments::getInputDir() {
    return inputDir;
}

char* Arguments::getMirrorDir() {
    return mirrorDir;
}

int Arguments::getBufferSize() {
    return bufferSize;
}

char* Arguments::getLogFile() {
    return logFile;
}

int Arguments::getId() {
    return id;
}
