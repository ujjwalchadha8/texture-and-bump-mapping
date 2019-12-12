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
//    cout << "Initialized with file: " << filePath << endl;
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
    this->model = MatrixXf(4, 4);
    this->model <<  1,    0.,   0.,   0.,
            0.,   1,    0.,   0.,
            0.,   0.,   1,    0.,
            0.,   0.,   0.,   1.;
}

Vector3f getParams1(Vector2f p, float coord1_x, float coord1_y, float coord2_x, float coord2_y, float coord3_x, float coord3_y) {
    Matrix3f A_;
    Vector3f b_;
    A_ << coord1_x, coord2_x, coord3_x, coord1_y, coord2_y, coord3_y, 1, 1, 1;
    b_ << p(0), p(1), 1;

    Vector3f sol = A_.colPivHouseholderQr().solve(b_);
    return sol;
}

bool isInVector1(const vector<int>& summed, int idx)
{
    for(int i : summed){
        if(idx == i) return true;
    }
    return false;
}

vector<MatrixXf> Object::calculateDerivatives() {
    MatrixXf dna_faces_ou = MatrixXf::Zero(3, vertices.cols());
    MatrixXf dna_vertices_ou = MatrixXf::Zero(3, vertices.cols());

    MatrixXf dna_faces_ov = MatrixXf::Zero(3, vertices.cols());
    MatrixXf dna_vertices_ov = MatrixXf::Zero(3, vertices.cols());

    float epsilon = 0.1;
    for(int i = 0; i < uvs.cols(); i+=3)
    {
        int column = i;
        float coord_1_x = uvs.col(column)(0);
        float coord_1_y = uvs.col(column)(1);

        float coord_2_x = uvs.col(column+1)(0);
        float coord_2_y = uvs.col(column+1)(1);

        float coord_3_x = uvs.col(column+2)(0);
        float coord_3_y = uvs.col(column+2)(1);

        // Get the 3 points
//    Vector2f barycenter = calculateBarycenter(i);
        float barycenter_x = (coord_1_x + coord_2_x + coord_3_x) / 3;
        float barycenter_y = (coord_1_y + coord_2_y + coord_3_y) / 3;

        Vector2f barycenter(barycenter_x, barycenter_y);
        Vector2f p_u = barycenter + Vector2f(epsilon, 0.0);
        Vector2f p_v = barycenter + Vector2f(0.0, epsilon);


        // Get alpha, beta, gamma for the 3 2D points
        Vector3f p_params = getParams1(barycenter, coord_1_x, coord_1_y,  coord_2_x, coord_2_y,  coord_3_x, coord_3_y);
        Vector3f p_u_params = getParams1(p_u, coord_1_x, coord_1_y,  coord_2_x, coord_2_y,  coord_3_x, coord_3_y);
        Vector3f p_v_params = getParams1(p_v, coord_1_x, coord_1_y,  coord_2_x, coord_2_y,  coord_3_x, coord_3_y);

        Vector3f V_p = (p_params(0) * vertices.col(i)) + (p_params(1) * vertices.col(i + 1)) + (p_params(2) * vertices.col(i + 2));
        Vector3f V_u = (p_u_params(0) * vertices.col(i)) + (p_u_params(1) * vertices.col(i + 1)) + (p_u_params(2) * vertices.col(i + 2));
        Vector3f V_v = (p_v_params(0) * vertices.col(i)) + (p_v_params(1) * vertices.col(i + 1)) + (p_v_params(2) * vertices.col(i + 2));
        Vector3f O_u = (V_u - V_p).normalized();
        Vector3f O_v = (V_v - V_p).normalized();

        // Save values
        dna_faces_ou.col(i) << O_u;
        dna_faces_ou.col(i + 1) << O_u;
        dna_faces_ou.col(i + 2) << O_u;
        dna_vertices_ou.col(i) << O_u;
        dna_vertices_ou.col(i + 1) << O_u;
        dna_vertices_ou.col(i + 2) << O_u;

        dna_faces_ov.col(i) << O_v;
        dna_faces_ov.col(i + 1) << O_v;
        dna_faces_ov.col(i + 2) << O_v;
        dna_vertices_ov.col(i) << O_v;
        dna_vertices_ov.col(i + 1) << O_v;
        dna_vertices_ov.col(i + 2) << O_v;
    }
    cout << "Done with faces, doing vertices" << endl;
    // Iterate through all 3d vertices and sum partial derivatives
    for(int i = 0; i < vertices.cols(); i++)
    {
        vector<int> summed_ov;
        vector<int> summed_ou;
        Vector3f current = vertices.col(i);
        Vector3f sum_ou = dna_vertices_ou.col(i);
        Vector3f sum_ov = dna_vertices_ov.col(i);

        summed_ov.push_back(i);
        summed_ou.push_back(i);
        for(int j = 0; j < vertices.cols(); j++){
            if(i == j) continue;
            if(!isInVector1(summed_ou, j)){
                Vector3f other = vertices.col(j);
                if(other[0] == current[0] && other[1] == current[1] && other[2] == current[2] ){
                    sum_ou += dna_vertices_ou.col(j);
                    summed_ou.push_back(j);
                }
            }
            if(!isInVector1(summed_ov, j)){
                Vector3f other = vertices.col(j);
                if(other[0] == current[0] && other[1] == current[1] && other[2] == current[2]){
                    sum_ov += dna_vertices_ov.col(j);
                    summed_ov.push_back(j);
                }
            }
        }
        // normalize sums and insert into all vertex columns
        sum_ou = sum_ou.normalized();
        for(int k = 0; k < summed_ou.size(); k++){
            int idx = summed_ou[k];
            dna_vertices_ou.col(idx) = sum_ou;
        }
        sum_ov = sum_ov.normalized();
        for(int k = 0; k < summed_ov.size(); k++){
            int idx = summed_ov[k];
            dna_vertices_ov.col(idx) = sum_ov;
        }
        if((i % 1000) == 0){
            cout << "At index: " << i << "/8544" << endl;
        }

    } //end outer loop

    vector<MatrixXf> result;
    result.push_back(dna_vertices_ou);
    result.push_back(dna_vertices_ov);
    return result;
//        VBO_OU.update(dna_vertices_ou);
//        VBO_OV.update(dna_vertices_ov);

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