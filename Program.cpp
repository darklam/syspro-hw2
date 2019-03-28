//
// Created by John Lamis on 2019-03-25.
//

#include "Program.hpp"
#include "FileUtils.hpp"
#include "Arguments.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

int Program::mainLoop() {

    FileUtils fileUtils;
    Arguments* args = Arguments::getInstance();

    char* inputDir = args->getInputDir();
    char* mirrorDir = args->getMirrorDir();

    if (fileUtils.dirExists(mirrorDir)) {
        std::cout << "Mirror directory already exists\n";
        return 1;
    }

    if (!fileUtils.dirExists(inputDir)) {
        std::cout << "Input directory does not exist\n";
        return 1;
    }

    bool good = fileUtils.createIdFile();
//    if (!good) return 1;

    this->listenForInputChanges();


    return 0;
}

Program::Program() {
    this->processedIds = new int[10000];
    this->processedLength = 0;
}

Program::~Program() {
    delete[] this->processedIds;
}

void Program::listenForInputChanges() {
    unsigned int interval = 10; // seconds to wait until the directory is checked again
    FileUtils utils;
    while (true) {
        int* newIds = utils.getNewIds(this->processedIds, this->processedLength);
        int index = 0;
        int current = newIds[index++];
        while (current != 0) {
            if (current == Arguments::getInstance()->getId()) {
                current = newIds[index++];
                continue;
            }
            this->processedIds[this->processedLength++] = current;
            this->createNewProcessPair(current);
            current = newIds[index++];
        }
        delete[] newIds;
        std::cout << "Sleeping\n";
        sleep(interval);
    }
}

bool Program::handleFileWrites(List<FileDto*>* files) {

}

void Program::processReaderHandler(int id) {
    int currentId = Arguments::getInstance()->getId();
    char nameString[50];
    sprintf(nameString, "%d_to_%d.fifo", id, currentId);

    FileUtils utils;
    List<FileDto*>* files = utils.readPipeFiles(nameString);
    std::cout << files->getSize() << std::endl;

    bool status = this->handleFileWrites(files);

    if (files->getSize() > 0) files->clearValues();
    delete files;

    if (status) exit(0);
    else exit(1);
}

void Program::processWriterHandler(int id) {
    int currentId = Arguments::getInstance()->getId();
    char nameString[50];
    sprintf(nameString, "%d_to_%d.fifo", currentId, id);

    std::cout << "Writing to " << nameString << std::endl;

    FileUtils utils;

    List<FileDto*>* files = utils.readDirFiles(Arguments::getInstance()->getInputDir());

    utils.writePipeFiles(nameString, files);

    files->clearValues();
    delete files;

    exit(0);
}

void Program::createNewProcessPair(int id) {
    pid_t process1 = fork();

    int status1;
    int status2;

    if (process1 == 0) {
        this->processReaderHandler(id);
        exit(0);
    }

    wait(&status1);

    pid_t process2 = fork();

    if (process2 == 0) {
        this->processWriterHandler(id);
        exit(0);
    }

    wait(&status2);


    if (status1 != 0 || status2 != 0) {
//        std::cout << status1 << " " << status2 << std::endl;
        std::cout << "I should have used JavaScript\n";
    }
}