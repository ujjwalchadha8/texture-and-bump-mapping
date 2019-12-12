//
// Created by Ujjwal Chadha on 12/10/19.
//

#ifndef FINALPROJECT_SHADER_H
#define FINALPROJECT_SHADER_H


#include <string>

class Shader {
public:
    static std::string* loadShaderCodeAsString(const std::string& filePath);
};


#endif //FINALPROJECT_SHADER_H
