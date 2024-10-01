//
// Created by RainSure on 24-10-1.
//

#ifndef POISSONRECONCGAL_READ_BERP_H
#define POISSONRECONCGAL_READ_BERP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

namespace brep {
    struct Node {
        double x, y, z;
    };

    struct Edge {
        int start, end;
    };

    struct Face {
        int nodes[3];
        int edges[3];
        int leftVolume, rightVolume;
        int surfaceMarker;
    };

    struct Zone {
        int zoneNumber;
        std::vector<int> volumes;
    };

    struct Model {
        int osCode;
        std::string modelName;
        std::vector<Node> nodes;
        std::vector<Edge> edges;
        std::vector<Face> faces;
        std::vector<Zone> zones;
        std::vector<std::string> horizons;
        std::vector<std::string> faults;
        std::vector<std::string> boundaries;
    };


    /**
     * example:
     *  0
        EXTREME_MODEL
        1001
        5125 10253 8235 15
        8
        3 0 1 2
        1 3
        4 4 5 6 7
        5120.4597 4532.4566 1542.1063
        .
        .
        .
        7 123
        .
        .
        .
        1 8 121 54 4 25 0 0 ‐1
        .
        .
        .
        1
        0 2 0 1
     *
     *
     *
     * @brief
     * @param filename
     * @param model
     * @return
     */
    bool loadModel(const std::string& filename, Model& model) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return false;
        }

        std::string line;
        getline(file, line);
        model.osCode = std::stoi(line);

        getline(file, model.modelName);
        getline(file, line);  // Skip model identifier line ("1001" etc.)

        // Read dimensions
        getline(file, line);
        std::istringstream iss(line);
        int numNodes, numEdges, numFaces, numVolumes;
        iss >> numNodes >> numEdges >> numFaces >> numVolumes;

        // Read and skip markers
        getline(file, line); // Skip marker counts
        getline(file, line); // Read horizon markers
        getline(file, line); // Read fault markers
        getline(file, line); // Read boundary surface markers

        // Read nodes
        model.nodes.resize(numNodes);
        for (int i = 0; i < numNodes; ++i) {
            getline(file, line);
            std::istringstream iss(line);
            iss >> model.nodes[i].x >> model.nodes[i].y >> model.nodes[i].z;
        }

        // Read edges
        model.edges.resize(numEdges);
        for (int i = 0; i < numEdges; ++i) {
            getline(file, line);
            std::istringstream iss(line);
            iss >> model.edges[i].start >> model.edges[i].end;
        }

        // Read faces
        model.faces.resize(numFaces);
        for (int i = 0; i < numFaces; ++i) {
            getline(file, line);
            std::istringstream iss(line);
            iss >> model.faces[i].nodes[0] >> model.faces[i].nodes[1] >> model.faces[i].nodes[2];
            iss >> model.faces[i].edges[0] >> model.faces[i].edges[1] >> model.faces[i].edges[2];
            iss >> model.faces[i].leftVolume >> model.faces[i].rightVolume >> model.faces[i].surfaceMarker;
        }

        // Read zones
        getline(file, line); // Read number of zones
        int numZones = std::stoi(line);
        model.zones.resize(numZones);
        for (int i = 0; i < numZones; ++i) {
            getline(file, line);
            std::istringstream iss(line);
            iss >> model.zones[i].zoneNumber;
            int volume;
            while (iss >> volume) {
                model.zones[i].volumes.push_back(volume);
            }
        }

        file.close();
        return true;
    }
} // end namespace brep




#endif //POISSONRECONCGAL_READ_BERP_H
