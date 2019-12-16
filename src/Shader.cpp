//
// Created by Ujjwal Chadha on 12/10/19.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <streambuf>
#include "Shader.h"

using namespace std;

std::string *Shader::loadShaderCodeAsString(const std::string &filePath) {
    std::ifstream t(filePath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return new string(buffer.str());
}