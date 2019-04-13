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
#include <cstdlib>

int Program::mainLoop() {

    FileUtils fileUtils;
    Arguments* args = Arguments::getInstance();

    char* inputDir = args->getInputDir();
    char* mirrorDir = args->getMirrorDir();

//    if (fileUtils.dirExists(mirrorDir)) {
//        std::cout << "Mirror directory already exists\n";
//        return 1;
//    }

    fileUtils.createDirectory(mirrorDir);

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
    this->ids = new List<int>;
    this->processedLength = 0;
}

Program::~Program() {
    delete this->ids;
}

void Program::handleRemovedId(int id) {
    FileUtils f;
    char dir[300];
    sprintf(dir, "%s/%d", Arguments::getInstance()->getMirrorDir(), id);
    f.removeDirectory(dir);
}

void Program::listenForInputChanges() {
    unsigned int interval = 10; // seconds to wait until the directory is checked again
    FileUtils utils;
    while (true) {
        List<int>* newIds = utils.getIds();
        List<int>* idsToRemove = new List<int>;
        for (int i = 0; i < this->ids->getSize(); i++) {
            int currentId = this->ids->get(i);

            int index = newIds->findIndex(currentId);
            if (index == -1) {
                this->handleRemovedId(currentId);
                idsToRemove->push(i);
            }
        }

        for (int i = 0; i < idsToRemove->getSize(); i++) {
            int currentIndex = idsToRemove->get(i);
            this->ids->remove(currentIndex);
        }

        List<int>* handledIds = new List<int>;

        for (int i = 0; i < newIds->getSize(); i++) {
            int currentId = newIds->get(i);

            int index = this->ids->findIndex(currentId);

            if (index == -1) {
                this->createNewProcessPair(currentId);
                handledIds->push(currentId);
            }
        }

        for (int i = 0; i < handledIds->getSize(); i++) {
            int currentId = handledIds->get(i);

            this->ids->push(currentId);
        }

        delete newIds;
        delete idsToRemove;

        std::cout << "Sleeping" << std::endl;
        sleep(interval);
    }
}

bool Program::handleFileWrites(List<FileDto*>* files, int id) {

    FileUtils utils;

    char dirname[200];
    sprintf(dirname, "%s/%d", Arguments::getInstance()->getMirrorDir(), id);

    utils.createDirectory(dirname);

    bool success = utils.writeFiles(files, id);

    if (!success) {
        std::cout << "There was an error writing files\n";
        _exit(1);
    }

    return true;
}

void Program::processReaderHandler(int id) {
    int currentId = Arguments::getInstance()->getId();
    char nameString[50];
    sprintf(nameString, "%d_to_%d.fifo", id, currentId);

    FileUtils utils;
    List<FileDto*>* files = utils.readPipeFiles(nameString);

    int counter = 0;

    while (files->getSize() == 0 && counter < 30) {
        files = utils.readPipeFiles(nameString);
        counter++;
        sleep(1);
    }

    bool status = this->handleFileWrites(files, id);

    if (files->getSize() > 0) files->clearValues();
    delete files;

    if (status) _exit(0);
    else _exit(1);
}

void Program::processWriterHandler(int id) {
    int currentId = Arguments::getInstance()->getId();
    char nameString[50];
    sprintf(nameString, "%d_to_%d.fifo", currentId, id);

    FileUtils utils;

    List<FileDto*>* files = utils.readDirFiles(Arguments::getInstance()->getInputDir());
    utils.writePipeFiles(nameString, files);

    if (files->getSize() > 0) files->clearValues();
    delete files;

    _exit(0);
}

void Program::createNewProcessPair(int id) {
    pid_t process1 = fork();

    int status1;
    int status2;

    if (process1 == 0) {
        this->processReaderHandler(id);
        _exit(0);
    }

    pid_t process2 = fork();

    if (process2 == 0) {
        this->processWriterHandler(id);
        _exit(0);
    }

    wait(&status1);
    wait(&status2);


    if (status1 != 0 || status2 != 0) {
        std::cout << status1 << " " << status2 << std::endl;
        std::cout << "I should have used JavaScript\n";
    }
}