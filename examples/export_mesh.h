//
// Created by RainSure on 24-8-23.
//

#ifndef POISSONRECONCGAL_EXPORT_MESH_H
#define POISSONRECONCGAL_EXPORT_MESH_H

#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include "geometry/vector3.h"

void export_to_obj(const std::string& input, const std::string& output)
{
    std::ifstream input_file(input);
    if (!input_file)
    {
        std::cerr << "Error: cannot read file input file!" << std::endl;
        return;
    }
    std::vector<Vector3> points;
    while(input_file)
    {
        Vector3 p;
        input_file >> p.x >> p.y >> p.z;
        if (input_file)
        {
            points.push_back(p);
        }
    }

    std::ofstream output_file(output);
    if (!output_file)
    {
        std::cerr << "Error: cannot write file output file!" << std::endl;
        return;
    }
    for (const auto& p : points)
    {
        output_file << "v " << p.x << " " << p.y << " " << p.z << std::endl;
    }
    for(size_t i = 1; i <= points.size(); i += 3)
    {
        if (i + 2 >= points.size()) break;
        output_file << "f " << i << " " << i + 1 << " " << i + 2 << std::endl;
    }
    output_file.close();
}

#endif //POISSONRECONCGAL_EXPORT_MESH_H
