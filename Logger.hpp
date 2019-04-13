//
// Created by John Lamis on 2019-04-13.
//

#ifndef MIRRORCLIENT_LOGGER_HPP
#define MIRRORCLIENT_LOGGER_HPP


class Logger {
private:
    static Logger* instance;
public:
    static Logger* getInstance();
    void log(char* message);
};


#endif //MIRRORCLIENT_LOGGER_HPP
