//
// Created by John Lamis on 2019-03-25.
//

#ifndef MIRRORCLIENT_ARGUMENTS_HPP
#define MIRRORCLIENT_ARGUMENTS_HPP


class Arguments {
private:
    Arguments();
    static Arguments* instance;
    char* commonDir;
    char* inputDir;
    char* mirrorDir;
    int bufferSize;
    char* logFile;
    int id;

public:
    static Arguments* getInstance();
    void initialize(int, char**);

    char* getCommonDir();

    char* getInputDir();

    char* getMirrorDir();

    int getBufferSize();

    char* getLogFile();

    int getId();

};


#endif //MIRRORCLIENT_ARGUMENTS_HPP
