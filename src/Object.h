//
// Created by Ujjwal Chadha on 12/11/19.
//

#ifndef FINALPROJECT_OBJECT_H
#define FINALPROJECT_OBJECT_H

#include <iostream>
#include <Eigen/Core>
#include <vector>

using namespace std;
using namespace Eigen;

class Object {
private:
    MatrixXf vertices;
    MatrixXf uvs;
    MatrixXf normals;

public:
    static Object fromObjFile(const string& filePath);
    Object(const MatrixXf& vertices, const MatrixXf& uvs, const MatrixXf& normals);
    MatrixXf getVertices();
    MatrixXf getUVs();
    MatrixXf getNormals();
};


#endif //FINALPROJECT_OBJECT_H
