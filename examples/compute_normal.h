//
// Created by RainSure on 24-8-23.
//

#ifndef POISSONRECONCGAL_COMPUTE_NORMAL_H
#define POISSONRECONCGAL_COMPUTE_NORMAL_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/read_xyz_points.h>
#include <CGAL/property_map.h>
#include <CGAL/jet_estimate_normals.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

// kernel
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

// Simple geometric types
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;

// Point with normal vector stored in a std::pair.
typedef std::pair<Point, Vector> PointVectorPair;
typedef std::vector<PointVectorPair> PointList;

// Concurrency
typedef CGAL::Parallel_if_available_tag Concurrency_tag;


int compute_normal(const std::string& input, const std::string& output)
{
    std::ifstream stream(input);
    if (!stream)
    {
        std::cerr << "Error: cannot read file input file!" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<PointVectorPair> points;
    while(stream)
    {
        Point p;
        stream >> p;
        if (stream)
        {
            points.emplace_back(p, Vector(0, 0, 0));
        }
    }

    // Estimates normals direction.
    // Note: jet_estimate_normals() requires an iterator over points
    // + property maps to access each point's position and normal.
    CGAL::jet_estimate_normals<Concurrency_tag>
            (points,
             6,
             CGAL::parameters::point_map (CGAL::First_of_pair_property_map<PointVectorPair>()).
                     normal_map (CGAL::Second_of_pair_property_map<PointVectorPair>()));

    std::ofstream out(output);
    if (!out)
    {
        std::cerr << "Error: cannot write file output file!" << std::endl;
        return EXIT_FAILURE;
    }

    for (auto & point : points)
    {
        out << point.first << " " << point.second << std::endl;
    }

    out.close();

    return EXIT_SUCCESS;

}

#endif //POISSONRECONCGAL_COMPUTE_NORMAL_H
