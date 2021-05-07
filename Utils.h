//
// Created by Merjem Stulanovic
//

#ifndef MATRIXTEMPLATE_UTILS_H
#define MATRIXTEMPLATE_UTILS_H

#include <string>
#include <iostream>

class Utils {
public :
    static void error(const std::string &errorMessage) {
        std::cout << errorMessage << std::endl;
        throw std::runtime_error(errorMessage);
    }

    static unsigned ceilDiv(unsigned a, unsigned b) {
        return 1 + ((a - 1) / b);
    }

};
#endif //MATRIXTEMPLATE_UTILS_H
