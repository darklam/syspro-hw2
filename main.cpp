#include <iostream>
#include "Program.hpp"
#include "Arguments.hpp"
#include <signal.h>
#include <cstdio>
#include <cstdbool>

__volatile bool STOP = false;

void quitHandler(int sig) {
    std::cout << "Quitting\n";
    STOP = true;
}

int main(int argc, char** argv) {
    Arguments* args = Arguments::getInstance();
    args->initialize(argc, argv);

    Program p;
    int code = p.mainLoop();
    delete args;

    signal(SIGINT, quitHandler);
    signal(SIGQUIT, quitHandler);

    while(true) {
        if (STOP) {
            break;
        }
    }


    return code;
}