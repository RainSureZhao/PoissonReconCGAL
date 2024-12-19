//
// Created by RainSure on 24-12-19.
//

#ifndef POISSONRECONCGAL_READ_BOUNDARY_REPRESENTATION_H
#define POISSONRECONCGAL_READ_BOUNDARY_REPRESENTATION_H

#include "model/geometry_element.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace utils {
    model::BoundaryRepresentation ReadBoundaryRepresentation(const std::string& filename) {
        std::ifstream brep_in(filename);
        if (!brep_in.is_open()) {
            std::cerr << "Error opening .brep file: " << filename << std::endl;
            throw std::runtime_error("Error opening .brep file");
        }
        model::BoundaryRepresentation brep;

        std::string line;

        // 读取操作系统代码，模型信息，唯一标识
        std::getline(brep_in, line);  // 读取操作系统代码
        std::istringstream(line) >> brep.osCode;
        std::getline(brep_in, brep.modelTag);  // 读取模型标签
        std::getline(brep_in, brep.uniqueId);  // 读取唯一标识

        // 读取计数和几何信息
        std::getline(brep_in, line);
        std::istringstream(line) >> brep.numVertices >> brep.numEdges >> brep.numFaces >> brep.numVolumes;
        std::getline(brep_in, line);
        // 读入marker的数量
        std::istringstream(line) >> brep.numMarkers;

        brep_in >> brep.numHorizonMarkers;
        std::vector<int> horizonMarkers(brep.numHorizonMarkers);
        for(int i = 0; i < brep.numHorizonMarkers; i ++) {
            int marker;
            brep_in >> marker;
            horizonMarkers[i] = marker;
            brep.horizonMarkers.insert(marker);
        }
        // fault的marker
        brep_in >> brep.numFaultMarkers;
        std::vector<int> faultMarkers(brep.numFaultMarkers);
        for(int i = 0; i < brep.numFaultMarkers; i ++) {
            int marker;
            brep_in >> marker;
            faultMarkers[i] = marker;
            brep.faultMarkers.insert(marker);
        }
        // boundary surface的marker
        brep_in >> brep.numBoundarySurfaceMarkers;
        std::vector<int> boundarySurfaceMarkers(brep.numBoundarySurfaceMarkers);
        for(int i = 0; i < brep.numBoundarySurfaceMarkers; i ++) {
            int marker;
            brep_in >> marker;
            boundarySurfaceMarkers[i] = marker;
            brep.boundaryMarkers.insert(marker);
        }

        // 读取顶点
        brep.vertices.resize(brep.numVertices);
        for (int i = 0; i < brep.numVertices; ++i) {
            brep_in >> brep.vertices[i].x >> brep.vertices[i].y >> brep.vertices[i].z;
        }

        // 读取边
        brep.edges.resize(brep.numEdges);
        for (int i = 0; i < brep.numEdges; ++i) {
            brep_in >> brep.edges[i].start >> brep.edges[i].end;
        }

        // 读取面
        brep.faces.resize(brep.numFaces);
        for (int i = 0; i < brep.numFaces; ++i) {
            brep.faces[i].vertices.resize(3);
            for (int j = 0; j < 3; ++j) {
                brep_in >> brep.faces[i].vertices[j];
            }
            brep.faces[i].verticesPositions.resize(3);
            for (int j = 0; j < 3; ++j) {
                brep.faces[i].verticesPositions[j] = brep.vertices[brep.faces[i].vertices[j]];
            }
            brep.faces[i].edges.resize(3);
            for (int j = 0; j < 3; ++j) {
                brep_in >> brep.faces[i].edges[j];
            }
            brep_in >> brep.faces[i].marker;  // 读取面的标记

            brep_in >> brep.faces[i].leftVolumeNumber; // 读取面左侧的体编号
            brep_in >> brep.faces[i].rightVolumeNumber; // 读取面右侧的体编号
            if(brep.boundaryMarkers.contains(brep.faces[i].marker)) brep.boundaryFaces.push_back(brep.faces[i]);
            if(brep.faultMarkers.contains(brep.faces[i].marker)) brep.faultFaces.push_back(brep.faces[i]);
            if(brep.horizonMarkers.contains(brep.faces[i].marker)) brep.horizonFaces.push_back(brep.faces[i]);
        }

        // 读取区域信息
        brep_in >> brep.numZones;  // 读取区域数量
        brep.zones.resize(brep.numZones);
        for (int i = 0; i < brep.numZones; ++i) {
            int numVolumesInZone;
            brep_in >> brep.zones[i].zoneNumber >> numVolumesInZone;
            brep.zones[i].volumes.resize(numVolumesInZone);
            for (int j = 0; j < numVolumesInZone; ++j) {
                brep_in >> brep.zones[i].volumes[j];
            }
        }
        return brep;
    }
}

#endif //POISSONRECONCGAL_READ_BOUNDARY_REPRESENTATION_H
