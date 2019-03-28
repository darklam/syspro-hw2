//
// Created by John Lamis on 2019-03-25.
//

#ifndef MIRRORCLIENT_PROGRAM_HPP
#define MIRRORCLIENT_PROGRAM_HPP


#include "FileUtils.hpp"

class Program {

private:
    int* processedIds;
    int processedLength;

public:
    Program();
    ~Program();
    int mainLoop();
    void listenForInputChanges();

    void createNewProcessPair(int id);

    void processReaderHandler(int id);
    void processWriterHandler(int id);

    bool handleFileWrites(List<FileDto*>* files);
};


#endif //MIRRORCLIENT_PROGRAM_HPP
