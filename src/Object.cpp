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
#include <unordered_map>

using namespace std;
using namespace Eigen;

Object Object::fromObjFile(const string &filePath) {
    vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;
    vector<Vector3f> allVertices;
    vector<Vector2f> allTexCoords;
    vector<Vector3f> allNormals;

    string line;
    ifstream stream(filePath.c_str());
    getline(stream,line);

    while(stream){
        getline(stream,line);
        if(line.length() == 0) continue;
        if(line[0] == 'v'){
            if(line[1] == 't'){ //TEXTURE
                vector<float> values = Utils::split_line(line, 3);
                allTexCoords.push_back(Vector2f(values[0], values[1]));
            } else if(line[1] == 'n') { //NORMAL
                vector<float> values = Utils::split_line(line, 3);
                Vector3f normal(values[0], values[1], values[2]);
                allNormals.push_back(normal);
            } else { //VERTEX
                vector<float> values = Utils::split_line(line, 2);
                Vector3f vertex(values[0], values[1], values[2]);
                allVertices.push_back(vertex);
            }
        }else if(line[0] == 'f'){ //FACE
            vector<float> data = Utils::split_face_line(line, 2);
            for(long i = 0; i < data.size(); i+= 3) {
                vertexIndices.push_back(data[i]);
                texCoordIndices.push_back(data[i + 1]);
                normalIndices.push_back(data[i + 2]);
            }
        }
    }

    MatrixXf vertices(3, vertexIndices.size()), normals(3, vertexIndices.size());
    MatrixXf texCoords(2, vertexIndices.size());
    for(long i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i] - 1;
        unsigned int uvIndex = texCoordIndices[i] - 1;
        unsigned int normalIndex = normalIndices[i] - 1;

        vertices.col(i) << allVertices[vertexIndex];
        texCoords.col(i) << allTexCoords[uvIndex];
        normals.col(i) << allNormals[normalIndex];
    }
    return Object(vertices, texCoords, normals);
}

Object::Object(const MatrixXf& vertices, const MatrixXf& uvs, const MatrixXf& normals) {
    this->vertices = vertices;
    this->textureCoordinates = uvs;
    this->normals = normals;
    this->model = MatrixXf(4, 4);
    this->model <<  1,    0.,   0.,   0.,
            0.,   1,    0.,   0.,
            0.,   0.,   1,    0.,
            0.,   0.,   0.,   1.;
}

Vector3f getIntersection(Vector2f p, float aX, float aY, float bX, float bY, float cX, float cY) {
    Matrix3f A;
    Vector3f b;
    A << aX, bX, cX, aY, bY, cY, 1, 1, 1;
    b << p(0), p(1), 1;

    return A.colPivHouseholderQr().solve(b);
}

template<typename T>
struct matrix_hash : std::unary_function<T, size_t> {
    std::size_t operator()(T const& matrix) const {
        size_t seed = 0;
        for (size_t i = 0; i < matrix.size(); ++i) {
            auto elem = *(matrix.data() + i);
            seed ^= std::hash<typename T::Scalar>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

vector<MatrixXf> Object::calculateTangentBasis() {
    MatrixXf facesOu = MatrixXf::Zero(3, vertices.cols());
    MatrixXf verticesOu = MatrixXf::Zero(3, vertices.cols());

    MatrixXf facesOv = MatrixXf::Zero(3, vertices.cols());
    MatrixXf verticesOv = MatrixXf::Zero(3, vertices.cols());

    float epsilon = 0.1;
    for(long i = 0; i < textureCoordinates.cols(); i+=3)
    {
        int column = i;
        float aX = textureCoordinates.col(column)(0);
        float aY = textureCoordinates.col(column)(1);

        float bX = textureCoordinates.col(column + 1)(0);
        float bY = textureCoordinates.col(column + 1)(1);

        float cX = textureCoordinates.col(column + 2)(0);
        float cY = textureCoordinates.col(column + 2)(1);

        Vector2f barycenter((aX + bX + cX) / 3, (aY + bY + cY) / 3);

        Vector3f p_params = getIntersection(barycenter, aX, aY, bX, bY, cX, cY);
        Vector3f p_u_params = getIntersection(barycenter + Vector2f(epsilon, 0.0), aX, aY, bX, bY, cX, cY);
        Vector3f p_v_params = getIntersection(barycenter + Vector2f(0.0, epsilon), aX, aY, bX, bY, cX, cY);

        Vector3f V_p = (p_params(0) * vertices.col(i)) + (p_params(1) * vertices.col(i + 1)) + (p_params(2) * vertices.col(i + 2));
        Vector3f V_u = (p_u_params(0) * vertices.col(i)) + (p_u_params(1) * vertices.col(i + 1)) + (p_u_params(2) * vertices.col(i + 2));
        Vector3f V_v = (p_v_params(0) * vertices.col(i)) + (p_v_params(1) * vertices.col(i + 1)) + (p_v_params(2) * vertices.col(i + 2));
        Vector3f O_u = (V_u - V_p).normalized();
        Vector3f O_v = (V_v - V_p).normalized();

        // Save values
        facesOu.col(i) << O_u;
        facesOu.col(i + 1) << O_u;
        facesOu.col(i + 2) << O_u;
        verticesOu.col(i) << O_u;
        verticesOu.col(i + 1) << O_u;
        verticesOu.col(i + 2) << O_u;

        facesOv.col(i) << O_v;
        facesOv.col(i + 1) << O_v;
        facesOv.col(i + 2) << O_v;
        verticesOv.col(i) << O_v;
        verticesOv.col(i + 1) << O_v;
        verticesOv.col(i + 2) << O_v;
    }

    unordered_map<Vector3f, vector<int>, matrix_hash<Vector3f>> vertexToIndexMap;
    for(long i = 0; i < vertices.cols(); i++) {
        if (vertexToIndexMap.find(vertices.col(i)) == vertexToIndexMap.end()) {
            vertexToIndexMap[vertices.col(i)] = std::vector<int>();
        }
        vertexToIndexMap[vertices.col(i)].push_back(i);
    }
    for(long i = 0; i < vertices.cols(); i++) {
        Vector3f sum_ou = verticesOu.col(i);
        Vector3f sum_ov = verticesOv.col(i);
        for (int otherVertexIndex: vertexToIndexMap[vertices.col(i)]) {
            sum_ou += verticesOu.col(otherVertexIndex);
            sum_ov += verticesOv.col(otherVertexIndex);
        }
        for (int otherVertexIndex: vertexToIndexMap[vertices.col(i)]) {
            verticesOu.col(otherVertexIndex) = sum_ou;
            verticesOv.col(otherVertexIndex) = sum_ov;
        }
    }

    vector<MatrixXf> result;
    result.push_back(verticesOu);
    result.push_back(verticesOv);
    return result;
}



MatrixXf Object::getVertices() {
    return this->vertices;
}

MatrixXf Object::getTextureCoordinates() {
    return this->textureCoordinates;
}

MatrixXf Object::getNormals() {
    return this->normals;
}

MatrixXf Object::getModel() {
    return this->model;
}

Vector3f Object::getTranslation() {
    auto res = Vector3f(this->model.block(0, 3, 3, 1));
    return res;
}

void Object::translate(const Vector3f& translateBy) {
    this->model << Utils::generateTranslationMatrix(translateBy) * this->model;
}

void Object::scale(float factor) {
    this->model << Utils::generateScaleAboutPointMatrix(getTranslation(), factor) * this->model;
}

void Object::rotate(int axis, float radians) {
    this->model << Utils::generateRotateAboutPointMatrix(axis, radians, getTranslation()) * this->model;
}