//
// Created by John Lamis on 2019-03-25.
//

#include "FileUtils.hpp"
#include "Arguments.hpp"
#include "List.hpp"
#include "Base64.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string.h>

bool FileUtils::dirExists(char *dirname) {
    struct stat sb;

    if (stat(dirname, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    }

    return false;
}

// creates a directory if it does not exist

bool FileUtils::createDirectory(char *dirname) {
    if (this->dirExists(dirname)) return true;

    mode_t nMode = 0733; // UNIX permission stuffz

    int error = mkdir(dirname, nMode);

    if (error) {
        std::cout << "failed to create dir " << dirname << std::endl;
        return false;
    }

    return true;
}

bool FileUtils::fileExists(char *filename) {
    struct stat buffer;

    if (stat(filename, &buffer) == 0) return true;
    return false;
}

bool FileUtils::createIdFile() {
    std::ofstream file;
    Arguments* args = Arguments::getInstance();
    this->createDirectory(args->getCommonDir());
    char filename[100];
    sprintf(filename, "%s/%d.id", args->getCommonDir(), args->getId());
    if (this->fileExists(filename)) {
        std::cout << "Lockfile already exists\n";
        return false;
    }
    file.open(filename, std::fstream::app);

    file << getpid();
    file.close();
    return true;
}

int* FileUtils::getNewIds(int* currentIds, int length) {
    char* dir = Arguments::getInstance()->getCommonDir();
    struct dirent* entry;
    DIR* directory = opendir(dir);

    int bufferLength = 1000;

    int* newIds = new int[bufferLength];

    int len = 0;

    for (int i = 0; i < bufferLength; i++) {
        newIds[i] = 0; // set to 0 so we know when to stop searching for ids later
    }

    if (directory == NULL) return NULL;

    while ((entry = readdir(directory)) != NULL) {
        char* name = entry->d_name;
        char* idString = strtok(name, ".");
        if (idString == NULL) continue;
        int id = atoi(idString);
        if (id == 0) continue;
        bool found = false;

        for (int i = 0; i < length; i++) {
            if (id == currentIds[i]) found = true;
        }

        if (!found) newIds[len++] = id;
    }
    closedir(directory);
    return newIds;
}

List<FileDto*>* FileUtils::readPipeFiles(char* pipename) {
    List<FileDto*>* out = new List<FileDto*>();

    char filepath[100];
    char* commonDir = Arguments::getInstance()->getCommonDir();
    sprintf(filepath, "%s/%s", commonDir, pipename);

    if (!this->fileExists(filepath)) return out;

    std::fstream file;
    file.open(filepath);

    while (file.good()) {
        int filenameLength;
        int fileLength;

        file >> filenameLength;
        if (filenameLength == 0) break;
        char* filename = new char[filenameLength + 1];
        file >> filename;

        file >> fileLength;
        char* fileBytes = new char[fileLength + 1];
        file >> fileBytes;

        FileDto* fileDto = new FileDto;
        fileDto->filename = filename;
        fileDto->fileBytes = fileBytes;
        fileDto->fileLength = fileLength;
        fileDto->nameLength = filenameLength;

        out->push(fileDto);
    }

    file.close();

    return out;
}

FileDto* FileUtils::getFile(char* dir) {
    FILE* file;
    char* bytes;
    int byteLength;

    file = fopen(dir, "rb");
    fseek(file, 0, SEEK_END);
    byteLength = ftell(file);
    rewind(file);

    std::cout << byteLength << std::endl;
    bytes = new char[byteLength];

    fread(bytes, byteLength, 1, file);
    fclose(file);

    char* encoded;
    size_t encodedlen;
    Base64 b;

    encoded = b.encode(bytes, byteLength, &encodedlen);

    FileDto* fileDto = new FileDto;
    fileDto->fileBytes = encoded;
    fileDto->fileLength = encodedlen;

    return fileDto;
}

void FileUtils::writePipeFiles(char* pipename, List<FileDto*>* files) {
    char filepath[100];
    char* commonDir = Arguments::getInstance()->getCommonDir();
    sprintf(filepath, "%s/%s", commonDir, pipename);
    std::cout << filepath << std::endl;
    std::ofstream file;

    file.open(filepath);

    int fileLength = files->getSize();

    for (int i = 0; i < fileLength; i++) {
        FileDto* current = files->get(i);

        file << current->nameLength << "\n";
        file << current->filename << "\n";
        file << current->fileLength << "\n";
        file << current->fileBytes << "\n";
    }

    file << "00\n";

    file.close();
}

List<FileDto*>* FileUtils::readDirFiles(char* dir) {
    DIR* directory;
    struct dirent *ent;

    List<FileDto*>* out = new List<FileDto*>();

    if ((directory = opendir(dir)) == NULL) return out;

    while ((ent = readdir(directory)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0) continue;
        if (strcmp(ent->d_name, "..") == 0) continue;

        char fullname[100];
        sprintf(fullname, "%s/%s", dir, ent->d_name);
        std::cout << fullname << std::endl;
        FileDto* current = this->getFile(fullname);
        current->filename = ent->d_name;
        current->nameLength = strlen(ent->d_name);

        out->push(current);
    }
    std::cout << out->getSize() << std::endl;
    return out;
}

FileDto::~FileDto() {
    delete[] this->fileBytes;
}
