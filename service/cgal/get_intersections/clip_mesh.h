//
// Created by ByteDance on 12/15/24.
//

#ifndef CLIP_MESH_H
#define CLIP_MESH_H

#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/intersection.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <iostream>
#include <fstream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> Surface_mesh;
typedef CGAL::Polyhedron_3<K> Polyhedron;
typedef typename boost::graph_traits<Surface_mesh>::face_descriptor    face_descriptor;
typedef Surface_mesh::Vertex_index vertex_descriptor;



#endif //CLIP_MESH_H
