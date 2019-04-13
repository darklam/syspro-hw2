//
// Created by John Lamis on 2019-03-25.
//

#ifndef MIRRORCLIENT_PROGRAM_HPP
#define MIRRORCLIENT_PROGRAM_HPP


#include "FileUtils.hpp"

class Program {

private:
    List<int>* ids;
    int processedLength;

public:
    Program();
    ~Program();
    int mainLoop();
    void listenForInputChanges();

    void createNewProcessPair(int id);

    void processReaderHandler(int id);
    void processWriterHandler(int id);
    void handleRemovedId(int id);

    bool handleFileWrites(List<FileDto*>* files, int id);
};


#endif //MIRRORCLIENT_PROGRAM_HPP
