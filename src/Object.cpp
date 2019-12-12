//
// Created by Ujjwal Chadha on 12/11/19.
//

#include "Object.h"
#include "Helpers.h"
#include "Utils.h"

// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

// Linear Algebra Library
#include <Eigen/Core>

#include <fstream>

// For A x = b solversolver
#include <Eigen/Dense>

#include <Eigen/Geometry>

#include <iostream>
#include <fstream>

#include <vector>


#include <cmath>

#include <math.h>

using namespace std;
using namespace Eigen;

Object Object::fromObjFile(const string &filePath) {
    cout << "Reading OBJ file: " << filePath << endl;
    // Data holders
    vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    vector< Vector3f > temp_vertices;
    vector< Vector2f > temp_uvs;
    vector< Vector3f > temp_normals;

    // Create file stream
    string line;
    ifstream stream(filePath.c_str());
    getline(stream,line);

    // Parse out beginning comments/new lines
    while(line[0] == '#'|| line.length() == 0){
        getline(stream,line);
    }
    while(stream){
        getline(stream,line);
        if(line.length() == 0) continue;
        if(line[0] == 'v'){
            if(line[1] == 't'){ //TEXTURE
                vector<float> values = Utils::split_line(line, 3);
                Vector2f uv(values[0], values[1]);
                temp_uvs.push_back(uv);
            }else if(line[1] == 'n'){ //NORMAL
                vector<float> values = Utils::split_line(line, 3);
                Vector3f normal(values[0], values[1], values[2]);
                temp_normals.push_back(normal);

            }else{ //VERTEX
                vector<float> values = Utils::split_line(line, 2);
                Vector3f vertex(values[0], values[1], values[2]);

                temp_vertices.push_back(vertex);
            }
        }else if(line[0] == 'f'){ //FACE
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            vector<float> data = Utils::split_face_line(line, 2);
            // Vertex, Texture UV, Normal
            for(long i = 0; i < data.size(); i+= 3)
            {
                vertexIndices.push_back(data[i]);
                uvIndices.push_back(data[i + 1]);
                normalIndices.push_back(data[i + 2]);
            }
        } //end face if
    } //end while
    // Use index values to format data correctly
    MatrixXf vertices(3, vertexIndices.size()), normals(3, vertexIndices.size());
    MatrixXf uvs(2, vertexIndices.size());
    for(long i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i] - 1;
        unsigned int uvIndex = uvIndices[i] - 1;
        unsigned int normalIndex = normalIndices[i] - 1;

        Vector3f vertex = temp_vertices[vertexIndex];
        Vector2f uv = temp_uvs[uvIndex];
        Vector3f normal = temp_normals[normalIndex];

        vertices.col(i) << vertex;
        uvs.col(i) << uv;
        normals.col(i) << normal;
    }
    return Object(vertices, uvs, normals);
}

Object::Object(const MatrixXf& vertices, const MatrixXf& uvs, const MatrixXf& normals) {
    this->vertices = vertices;
    this->uvs = uvs;
    this->normals = normals;
}

MatrixXf Object::getVertices() {
    return this->vertices;
}

MatrixXf Object::getUVs() {
    return this->uvs;
}

MatrixXf Object::getNormals() {
    return this->normals;
}