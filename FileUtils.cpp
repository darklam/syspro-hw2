//
// Created by John Lamis on 2019-03-25.
//

#include "FileUtils.hpp"
#include "Arguments.hpp"
#include "List.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string.h>
#include <sstream>
#include <fcntl.h>
#include <sys/types.h>

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
        char* extension = strtok(NULL, ".");
        if (strcmp(extension, "fifo") == 0) continue;
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
    int fd = open(filepath, O_RDONLY);

    if (!this->fileExists(filepath)) return out;

    std::fstream file;
    file.open(filepath, std::fstream::in | std::fstream::binary);
    file.seekg(0, std::fstream::end);
    int length = file.tellg();
    file.seekg(0, std::fstream::beg);
    char* bytes = new char[length];
    file.read(bytes, length);
    file.close();
    int index = 0;
    while (index < length) {
        char nameBytes[] = { bytes[index], bytes[index + 1] };
        index += 2;
        std::stringstream ss;
        ss << std::hex << nameBytes;
        int nameLength;
        ss >> nameLength;
        std::cout << nameLength << std::endl;
        if (nameLength == 0) break;
        char* name = new char[nameLength];
        for (int i = 0; i < nameLength; i++) {
            name[i] = bytes[index++];
        }
        std::cout << name << std::endl;
        char fileLengthBytes[] = { bytes[index], bytes[index + 1], bytes[index + 2], bytes[index + 3] };
        index += 4;
        std::stringstream ss1;
        ss1 << std::hex << fileLengthBytes;
        int fileLength;
        ss1 >> fileLength;
        char* fileBytes = new char[fileLength];
        for (int i = 0; i < fileLength; i++) {
            fileBytes[i] = bytes[index++];
        }
        FileDto* current = new FileDto;
        current->nameLength = nameLength;
        current->filename = name;
        current->fileLength = fileLength;
        current->fileBytes = fileBytes;
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
        char nameLengthBytes[2];
        char fileLengthBytes[4];
        sprintf(nameLengthBytes, "%x", (short) current->nameLength);
        sprintf(fileLengthBytes, "%x", current->fileLength);
        file.write(nameLengthBytes, 2);
        file.write(current->filename, current->nameLength);
        file.write(fileLengthBytes, 4);
        file.write(current->fileBytes, current->fileLength);
    }

    file.write((char[]){ 0, 0 }, 2);

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

void FileUtils::readDir(char *dir, char* relativeDir, List<FileDto*>* files) {
    DIR* directory;
    struct dirent* ent;

    if ((directory = opendir(dir)) == NULL) return;

    while((ent = readdir(directory)) != NULL) {
        if (ent->d_name[0] == '.') continue;

        char fullname[100];
        sprintf(fullname, "%s/%s", dir, ent->d_name);
        char* relativeName = new char[100];
        sprintf(relativeName, "%s/%s", relativeDir, ent->d_name);
        if (this->isDirecory(fullname)) {
            readDir(fullname, relativeName, files);
        } else {
            FileDto* current = this->getFile(fullname);
            current->filename = relativeName;
            current->nameLength = strlen(relativeName);
            files->push(current);
        }
    }
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
            readDir(fullname, ent->d_name, out);
            continue;
        }
        FileDto* current = this->getFile(fullname);
        current->filename = ent->d_name;
        current->nameLength = strlen(ent->d_name);

        out->push(current);
    }
    for (int i = 0; i < out->getSize(); i++) {
        std::cout << out->get(i)->filename << "  " << out->get(i)->fileLength << std::endl;
    }
    return out;
}

bool FileUtils::writeFile(FileDto* file) {
    std::fstream mFile;
    char filename[300];
    char* mirrorDir = Arguments::getInstance()->getMirrorDir();
    sprintf(filename, "%s/%s", mirrorDir , file->filename);
    this->makeDirectories(mirrorDir, file->filename);

    mFile.open(filename, std::fstream::binary | std::fstream::out);

    mFile.write(file->fileBytes, file->fileLength);

    mFile.close();

    return true;
}

bool FileUtils::writeFiles(List<FileDto*>* files) {

    for (int i = 0; i < files->getSize(); i++) {
        FileDto* file = files->get(i);

        bool success = this->writeFile(file);

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
