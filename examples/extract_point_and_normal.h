//
// Created by RainSure on 24-8-29.
//

#ifndef POISSONRECONCGAL_EXTRACT_POINT_AND_NORMAL_H
#define POISSONRECONCGAL_EXTRACT_POINT_AND_NORMAL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include "geometry/vector3.h"
#include "geometry/Face.h"

void extractPointsAndNormals(const std::string& objFilePath, const std::string& outputFilePath) {
    std::ifstream objFile(objFilePath);
    std::ofstream outFile(outputFilePath);

    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << objFilePath << std::endl;
        return;
    }
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
        return;
    }

    std::vector<Vector3> vertices;
    std::vector<Face> faces;

    std::string line;
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            double x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } else if (prefix == "f") {
            std::string v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            // 解析顶点索引，忽略法线索引
            int vertIndex1 = std::stoi(v1.substr(0, v1.find("//"))) - 1;
            int vertIndex2 = std::stoi(v2.substr(0, v2.find("//"))) - 1;
            int vertIndex3 = std::stoi(v3.substr(0, v3.find("//"))) - 1;

            faces.push_back({vertIndex1, vertIndex2, vertIndex3});
        }
    }

    for (const auto& face : faces) {
        const Vector3& v0 = vertices[face.v1];
        const Vector3& v1 = vertices[face.v2];
        const Vector3& v2 = vertices[face.v3];

        // Calculate the centroid
        Vector3 centroid = (v0 + v1 + v2) / 3.0;

        // Calculate the normal
        Vector3 normal = (v1 - v0).cross(v2 - v0).normalize();

        // Write the centroid and normal to the file
        outFile << centroid.x << " " << centroid.y << " " << centroid.z << " "
                << normal.x << " " << normal.y << " " << normal.z << "\n";
    }

    objFile.close();
    outFile.close();
}

#endif //POISSONRECONCGAL_EXTRACT_POINT_AND_NORMAL_H
