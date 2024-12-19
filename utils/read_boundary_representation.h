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
        int numVertices, numEdges, numFaces, numVolumes, numZones;
        int numMarkers;
        int numHorizonMarkers;
        int numFaultMarkers;
        int numBoundarySurfaceMarkers;
        std::getline(brep_in, line);
        std::istringstream(line) >> numVertices >> numEdges >> numFaces >> numVolumes;
        std::getline(brep_in, line);
        // 读入marker的数量
        std::istringstream(line) >> numMarkers;
        // horizon的marker
        std::unordered_set<int> horizonMarkerSet;
        brep_in >> numHorizonMarkers;
        std::vector<int> horizonMarkers(numHorizonMarkers);
        for(int i = 0; i < numHorizonMarkers; i ++) {
            int marker;
            brep_in >> marker;
            horizonMarkers[i] = marker;
            horizonMarkerSet.insert(marker);
        }
        // fault的marker
        brep_in >> numFaultMarkers;
        std::unordered_set<int> faultSurfaceMarkerSet;
        std::vector<int> faultMarkers(numFaultMarkers);
        for(int i = 0; i < numFaultMarkers; i ++) {
            int marker;
            brep_in >> marker;
            faultMarkers[i] = marker;
            faultSurfaceMarkerSet.insert(marker);
        }
        // boundary surface的marker
        brep_in >> numBoundarySurfaceMarkers;
        std::vector<int> boundarySurfaceMarkers(numBoundarySurfaceMarkers);
        std::unordered_set<int> boundarySurfaceMarkerSet;
        for(int i = 0; i < numBoundarySurfaceMarkers; i ++) {
            int marker;
            brep_in >> marker;
            boundarySurfaceMarkers[i] = marker;
            boundarySurfaceMarkerSet.insert(marker);
        }

        // 读取顶点
        std::vector<Vertex> vertices(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            brep_in >> vertices[i].x >> vertices[i].y >> vertices[i].z;
        }

        // 读取边
        std::vector<Edge> edges(numEdges);
        for (int i = 0; i < numEdges; ++i) {
            brep_in >> edges[i].start >> edges[i].end;
        }

        // 读取面
        std::vector<Face> boundarySurfaces;
        std::vector<Face> faultSurfaces;
        std::vector<Face> horizonSurfaces;
        std::vector<Face> faces(numFaces);
        for (int i = 0; i < numFaces; ++i) {
            faces[i].vertices.resize(3);
            for (int j = 0; j < 3; ++j) {
                brep_in >> faces[i].vertices[j];
            }
            faces[i].edges.resize(3);
            for (int j = 0; j < 3; ++j) {
                brep_in >> faces[i].edges[j];
            }
            brep_in >> faces[i].marker;  // 读取面的标记

            brep_in >> faces[i].leftVolumeNumber; // 读取面左侧的体编号
            brep_in >> faces[i].rightVolumeNumber; // 读取面右侧的体编号
            if(boundarySurfaceMarkerSet.count(faces[i].marker)) boundarySurfaces.push_back(faces[i]);
            if(faultSurfaceMarkerSet.count(faces[i].marker)) faultSurfaces.push_back(faces[i]);
            if(horizonMarkerSet.count(faces[i].marker)) horizonSurfaces.push_back(faces[i]);
        }

        // 读取区域信息
        brep_in >> numZones;  // 读取区域数量
        std::vector<Zone> zones(numZones);
        for (int i = 0; i < numZones; ++i) {
            int numVolumesInZone;
            brep_in >> zones[i].zoneNumber >> numVolumesInZone;
            zones[i].volumes.resize(numVolumesInZone);
            for (int j = 0; j < numVolumesInZone; ++j) {
                brep_in >> zones[i].volumes[j];
            }
        }
    }
}

#endif //POISSONRECONCGAL_READ_BOUNDARY_REPRESENTATION_H
