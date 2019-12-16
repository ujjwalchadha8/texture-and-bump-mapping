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
    MatrixXf textureCoordinates;
    MatrixXf normals;
    MatrixXf model;

public:
    static Object fromObjFile(const string& filePath);
    Object(const MatrixXf& vertices, const MatrixXf& uvs, const MatrixXf& normals);
    MatrixXf getVertices();
    MatrixXf getTextureCoordinates();
    MatrixXf getNormals();
    vector<MatrixXf> calculateTangentBasis();

    MatrixXf getModel();
    Vector3f getTranslation();
    void translate(const Vector3f& translateBy);
    void scale(float factor);
    void rotate(int axis, float radians);

};


#endif //FINAL_PROJECT_OBJECT_H
