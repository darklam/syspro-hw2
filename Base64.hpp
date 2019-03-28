//
// Created by John Lamis on 2019-03-29.
//

#ifndef MIRRORCLIENT_BASE64_HPP
#define MIRRORCLIENT_BASE64_HPP


class Base64 {

public:
    char* encode(char* , size_t, size_t*);
    unsigned char* decode(char*, size_t, size_t*);
    ~Base64();
};


#endif //MIRRORCLIENT_BASE64_HPP
