//
// Created by Ujjwal Chadha on 11/2/19.
//

#include "Utils.h"

#include <iostream>
#include <vector>

std::vector<std::string> Utils::splitString(std::string s, const std::string &delimiter) {
    size_t pos = 0;
    std::vector<std::string> tokens;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        tokens.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    if (!s.empty()) {
        tokens.push_back(s);
    }
    return tokens;
}

Eigen::MatrixXf Utils::generateScaleMatrix(float factor) {
    Eigen::MatrixXf transform(4, 4);
    transform <<  factor,    0.,   0.,   0.,
                    0., factor,    0.,   0.,
                    0.,   0., factor,    0.,
                    0.,   0.,   0.,      1.;
    return transform;
}

Eigen::MatrixXf Utils::generateRotationMatrix(int axis, float radians) {
    Eigen::MatrixXf transform(4, 4);
    if (axis == Utils::AXIS_Z){
        transform <<
                 cos(radians),      sin(radians),  0.,  0.,
                -sin(radians),     cos(radians),  0.,  0.,
                0.,                     0.,                 1.,  0.,
                0.,                     0.,                 0.,  1.;
    } else if(axis == Utils::AXIS_X){
        transform <<
                 1.,    0.,                  0.,                 0.,
                0.,    cos(radians),   sin(radians),  0.,
                0.,    -sin(radians),  cos(radians),  0.,
                0.,    0.,                  0.,                 1.;

    } else if(axis == Utils::AXIS_Y){
        transform <<
                 cos(radians),  0.,  -sin(radians),   0.,
                0.,                 1.,  0.,                   0.,
                sin(radians),  0.,  cos(radians),    0.,
                0.,                 0.,  0.,                   1.;
    }
    return transform;
}

Eigen::MatrixXf Utils::generateTranslationMatrix(const Eigen::Vector3f &translateBy) {
    Eigen::MatrixXf transform(4, 4);
    transform <<    1,    0.,   0.,   translateBy(0),
                    0.,   1,    0.,   translateBy(1),
                    0.,   0.,   1,    translateBy(2),
                    0.,   0.,   0.,   1.;
    return transform;
}

Eigen::MatrixXf Utils::generateScaleAboutPointMatrix(const Eigen::Vector3f& point, float factor) {
    Eigen::MatrixXf transform(4, 4);
    transform <<    factor,    0.,      0.,   point(0) * (1 - factor),
                    0.,     factor,     0.,   point(1) * (1 - factor),
                    0.,       0.,    factor,  point(2) * (1 - factor),
                    0.,       0.,       0.,       1;
    return transform;
}

bool Utils::rayTriangleIntersect(const Eigen::Vector3f &rayOrigin, const Eigen::Vector3f &rayVector, const Eigen::Vector3f &vertex0,
                                 const Eigen::Vector3f &vertex1, const Eigen::Vector3f &vertex2, float& t) {
    const float EPSILON = 0.00001;

    Eigen::Vector3f edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;

    h = rayVector.cross(edge2);
    a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = rayOrigin - vertex0;
    u = f * s.dot(h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = s.cross(edge1);
    v = f * rayVector.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return false;

    t = f * edge2.dot(q);
    if (t > EPSILON && t < 1/EPSILON) // ray intersection
    {
//        outIntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

Eigen::MatrixXf Utils::generateRotateAboutPointMatrix(int axis, float radians, const Eigen::Vector3f& center) {
    return generateTranslationMatrix(center) * generateRotationMatrix(axis, radians) * generateTranslationMatrix(-1*center);
}

std::vector<float> Utils::split_face_line(const std::string &line, int startIdx) {
    std::string extracted;
    std::vector<float> data;
    int z = startIdx;

    for(int i = z; i <= line.length(); i++){
        char val = line[i];
        if(val == '/' || val == ' '  || i == line.length()){ //convert to int and push
            data.push_back(atof(extracted.c_str()));
            extracted = "";
        }else{
            extracted.push_back(val);
        }
    }
    return data;
}


std::vector<float> Utils::split_line(const std::string& line, int startIdx) {
    std::string extracted;
    std::vector<float> data;
    int z = startIdx;

    for(int i = z; i <= line.length(); i++){

        char val = line[i];

        if(val == ' ' || i == line.length()){ // Finished building int
            // Convert to int and push to data vector
            data.push_back(atof(extracted.c_str()));
            extracted = "";
        }else{ // Still building int
            extracted.push_back(val);
        }
    }
    return data;
}

