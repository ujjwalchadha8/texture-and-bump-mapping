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

std::string* Shader::loadShaderCodeAsString(const std::string &filePath) {
    std::ifstream t(filePath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return new string(buffer.str());
//    std::string vertexCode;
//    std::string fragmentCode;
//    std::ifstream vShaderFile(filePath);
//    cout<<vShaderFile.good()<<endl;
////    std::ifstream fShaderFile;
//    // ensure ifstream objects can throw exceptions:
//    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
////    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    try {
//        cout<<"Opening file"<<endl;
////        vShaderFile.open(filePath);
////        fShaderFile.open(fragmentPath);
//        cout<<"Opened file"<<endl;
//        std::stringstream vShaderStream;
//        vShaderStream << vShaderFile.rdbuf();
//        cout<<"Read data"<<endl;
////        fShaderStream << fShaderFile.rdbuf();
//        vShaderFile.close();
////        fShaderFile.close();
//        return vShaderStream.str();
////        fragmentCode = fShaderStream.str();
//    } catch(std::ifstream::failure &e) {
//        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//        throw e;
//    }
}