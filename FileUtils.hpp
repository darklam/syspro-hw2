//
// Created by John Lamis on 2019-03-25.
//

#ifndef MIRRORCLIENT_FILEUTILS_HPP
#define MIRRORCLIENT_FILEUTILS_HPP

#include <unistd.h>
#include "List.hpp"

struct FileDto {
    int nameLength;
    char* filename;
    int fileLength;
    char* fileBytes;
    ~FileDto();
};

class FileUtils {

public:

    bool fileExists(char* filename);
    bool dirExists(char* dirname);
    bool createDirectory(char* dirname);
    bool createIdFile();
    List<int>* getIds();
    List<FileDto*>* readPipeFiles(char* pipename);
    void writePipeFiles(char* pipename, List<FileDto*>* files);
    bool isDirecory(char* path);
    List<FileDto*>* readDir(char* dir, char* relativeDir);
    void makeDirectories(char* baseDir, char* dirs);
    void removeDirectory(char* dir);

    List<FileDto*>* readDirFiles(char* dir);

    FileDto* getFile(char* dir);

    bool writeFiles(List<FileDto*>* files, int id);
    bool writeFile(FileDto* file, int id);

};


#endif //MIRRORCLIENT_FILEUTILS_HPP
