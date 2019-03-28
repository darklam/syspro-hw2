#include <iostream>
#include "Program.hpp"
#include "Arguments.hpp"

int main(int argc, char** argv) {
    Arguments* args = Arguments::getInstance();
    args->initialize(argc, argv);

    Program p;
    int code = p.mainLoop();

    delete args;
    return code;
}