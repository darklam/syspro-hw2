//
// Created by John Lamis on 2019-03-25.
//

#include "FileUtils.hpp"
#include "Arguments.hpp"
#include "List.hpp"
#include "Logger.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string.h>
#include <sstream>
#include <cstdio>

bool FileUtils::dirExists(char *dirname) {
    struct stat sb;

    if (stat(dirname, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return true;
    }

    return false;
}

void FileUtils::removeDirectory(char* dir) {
    char command[100];
    sprintf(command, "rm -rf %s", dir);
    system(command);
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

List<FileDto*>* FileUtils::readPipeFiles(char* pipename) {
    List<FileDto*>* out = new List<FileDto*>();

    char filepath[100];
    char* commonDir = Arguments::getInstance()->getCommonDir();
    sprintf(filepath, "%s/%s", commonDir, pipename);

    if (!this->fileExists(filepath)) return out;

    std::fstream file;
    file.open(filepath, std::fstream::in | std::fstream::binary);
    file.seekg(0, std::fstream::end);
    int length = file.tellg();
    file.seekg(0, std::fstream::beg);
    int index = 0;
    short nameLength = 1;
    while (nameLength != 0) {
        file.read((char*)&nameLength, sizeof(short));
        if (nameLength == 0) break;
        std::cout << nameLength << std::endl;
        char* name = new char[nameLength + 1];
        file.read(name, nameLength);
        name[nameLength] = '\0';
        int fileLength;
        file.read((char*) &fileLength, sizeof(int));
        char* bytes = new char[fileLength];
        file.read(bytes, fileLength);
        FileDto* current = new FileDto;
        char loggerMessage[500];
        sprintf(loggerMessage, "Found file %s with size %d and bytes:", name, fileLength);
        Logger::getInstance()->log(loggerMessage);
        Logger::getInstance()->log(bytes);
        current->nameLength = nameLength;
        current->filename = name;
        current->fileLength = fileLength;
        current->fileBytes = bytes;
        out->push(current);
    }
    return out;
}

List<int>* FileUtils::getIds() {
    char* dir = Arguments::getInstance()->getCommonDir();
    struct dirent* entry;
    DIR* directory = opendir(dir);

    List<int>* ids = new List<int>;

    if (directory == NULL) return NULL;

    while ((entry = readdir(directory)) != NULL) {
        char* name = entry->d_name;
        if (strcmp(name, ".") == 0) continue;
        if (strcmp(name, "..") == 0) continue;
        char* idString = strtok(name, ".");
        char* extension = strtok(NULL, ".");
        if (extension == NULL) continue;
        if (idString == NULL) continue;
        if (strcmp(extension, "fifo") == 0) continue;
        int id = atoi(idString);
        if (id == Arguments::getInstance()->getId()) continue;
        if (id == 0) continue;

        ids->push(id);
    }
    closedir(directory);
    return ids;
}

FileDto* FileUtils::getFile(char* dir) {
    FileDto* fileDto = new FileDto;

    std::fstream file;
    file.open(dir, std::fstream::in | std::fstream::binary);

    file.seekg(0, std::fstream::end);
    int length = file.tellg();
    file.seekg(0, std::fstream::beg);

    int bufferSize = Arguments::getInstance()->getBufferSize();
    if (length > bufferSize) length = bufferSize;

    char* bytes = new char[length];

    file.read(bytes, length);
    file.close();

    fileDto->fileLength = length;
    fileDto->fileBytes = bytes;

    return fileDto;
}

void FileUtils::writePipeFiles(char* pipename, List<FileDto*>* files) {
    char filepath[100];
    char* commonDir = Arguments::getInstance()->getCommonDir();
    sprintf(filepath, "%s/%s", commonDir, pipename);
    std::ofstream file;

    file.open(filepath, std::fstream::out | std::fstream::binary);

    int fileLength = files->getSize();

    for (int i = 0; i < fileLength; i++) {
        FileDto* current = files->get(i);
        short nameLen = current->nameLength;
        file.write((char*)&nameLen, sizeof(short));
        file.write(current->filename, current->nameLength);
        file.write((char*) &(current->fileLength), sizeof(int));
        file.write(current->fileBytes, current->fileLength);
    }

    short a = 0;

    file.write((char*) &a, 2);

    file.close();
}

void FileUtils::makeDirectories(char* baseDir, char* dirs) {
    List<char*>* folders = new List<char*>();
    char* strtokString = new char[strlen(dirs)];
    strcpy(strtokString, dirs);
    char* folder = strtok(strtokString, "/");
    folders->push(folder);
    while((folder = strtok(NULL, "/")) != NULL) folders->push(folder);
    char append[400];
    sprintf(append, "%s", baseDir);
    for (int i = 0; i < folders->getSize() - 1; i++) {
        char* fold = folders->get(i);
        sprintf(append, "%s/%s", append, fold);
        if (this->dirExists(append)) continue;
        this->createDirectory(append);
        sprintf(append, "%s/%s", append, fold);
    }
    delete folders;
    delete[] strtokString;
}

bool FileUtils::isDirecory(char *path) {
    struct stat s;
    if (stat(path, &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            return true;
        }
    }
    return false;
}

List<FileDto *>* FileUtils::readDir(char *dir, char* relativeDir) {
    DIR* directory;
    struct dirent* ent;
    List<FileDto*>* out = new List<FileDto*>();
    if ((directory = opendir(dir)) == NULL) return out;

    while((ent = readdir(directory)) != NULL) {
        if (ent->d_name[0] == '.') continue;

        char fullname[100];
        sprintf(fullname, "%s/%s", dir, ent->d_name);
        char* relativeName = new char[100];
        sprintf(relativeName, "%s/%s", relativeDir, ent->d_name);
        if (this->isDirecory(fullname)) {
            List<FileDto*>* files = readDir(fullname, relativeName);
            for (int i = 0; i < files->getSize(); i++) {
                out->push(files->get(i));
            }
            delete files;
        } else {
            FileDto* current = this->getFile(fullname);
            current->filename = relativeName;
            current->nameLength = strlen(relativeName);
            out->push(current);
        }
    }
    return out;
}

List<FileDto*>* FileUtils::readDirFiles(char* dir) {
    DIR* directory;
    struct dirent* ent;

    List<FileDto*>* out = new List<FileDto*>();

    if ((directory = opendir(dir)) == NULL) return out;

    while ((ent = readdir(directory)) != NULL) {
        if (ent->d_name[0] == '.') continue;

        char fullname[100];
        sprintf(fullname, "%s/%s", dir, ent->d_name);
        if (this->isDirecory(fullname)) {
            List<FileDto*>* dirFiles = readDir(fullname, ent->d_name);
            for (int i = 0; i < dirFiles->getSize(); i++) {
                out->push(dirFiles->get(i));
            }
            delete dirFiles;
            continue;
        }
        FileDto* current = this->getFile(fullname);
        char *fname = new char[strlen(ent->d_name) + 1];
        strcpy(fname, ent->d_name);
        current->filename = ent->d_name;
        current->nameLength = strlen(ent->d_name);

        out->push(current);
    }

    return out;
}

bool FileUtils::writeFile(FileDto* file, int id) {
    std::fstream mFile;
    char filename[300];
    char* mirrorDir = Arguments::getInstance()->getMirrorDir();
    char idDir[200];
    sprintf(idDir, "%s/%d", mirrorDir, id);
    sprintf(filename, "%s/%s", idDir, file->filename);
    this->makeDirectories(idDir, file->filename);

    mFile.open(filename, std::fstream::binary | std::fstream::out);

    mFile.write(file->fileBytes, file->fileLength);

    mFile.close();

    return true;
}

bool FileUtils::writeFiles(List<FileDto*>* files, int id) {

    for (int i = 0; i < files->getSize(); i++) {
        FileDto* file = files->get(i);

        bool success = this->writeFile(file, id);

        if (!success) {
            std::cout << "There was an error writing the file " << file->filename << std::endl;
            _exit(1);
        }
    }
    return true;
}

FileDto::~FileDto() {
    delete[] this->fileBytes;
}
