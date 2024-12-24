//
// Created by RainSure on 24-12-24.
//

#ifndef POISSONRECONCGAL_TET_SURFACE_MESH_WITHOUT_FEATURE_H
#define POISSONRECONCGAL_TET_SURFACE_MESH_WITHOUT_FEATURE_H
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>

#include <CGAL/make_mesh_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

namespace PMP = CGAL::Polygon_mesh_processing;
namespace params = CGAL::parameters;

namespace service::cgal::tetrahedron {

    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef CGAL::Polyhedron_3<K> Polyhedron;
    typedef K::FT FT;
    typedef K::Point_3 Point;
    typedef CGAL::Surface_mesh<Point> Surface_mesh;
    typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K> Polyhedral_Mesh_domain;

    typedef CGAL::Sequential_tag Concurrency_tag;

    typedef CGAL::Mesh_triangulation_3<Polyhedral_Mesh_domain,CGAL::Default,Concurrency_tag>::type Tr;
    typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

    typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
    typedef Mesh_criteria::Facet_criteria    Facet_criteria;
    typedef Mesh_criteria::Cell_criteria     Cell_criteria;
    /**
      * @brief 从表面网格生成四面体网格，表面网格必须要是三角网格
      * @param input 输入文件，xxx.ply xxx.off
      * @param output 输出文件 xxx.mesh
      */
    void tetrahedralization_by_surface_mesh(const std::string& fname, const std::string& output, const std::string& output_refinement) {
        // Create input polyhedron
        Polyhedron polyhedron;
        std::ifstream input(fname);
        input >> polyhedron;
        if(input.fail()){
            std::cerr << "Error: Cannot read file " <<  fname << std::endl;
            return;
        }
        input.close();

        if (!CGAL::is_triangle_mesh(polyhedron)){
            std::cerr << "Input geometry is not triangulated." << std::endl;
            return;
        }

        // Create domain
        Polyhedral_Mesh_domain domain(polyhedron);

        // Mesh criteria (no cell_size set)
        Mesh_criteria criteria(params::facet_angle(25).facet_size(10.0).facet_distance(8.0).
                cell_radius_edge_ratio(3));
        // Mesh generation
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, params::no_perturb().no_exude());

        // Output
        std::ofstream medit_file(output);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
        medit_file.close();

        // Set tetrahedron size (keep cell_radius_edge_ratio), ignore facets
        Mesh_criteria new_criteria(params::cell_radius_edge_ratio(3).cell_size(0.03));

        // Mesh refinement (and make the output manifold)
        CGAL::refine_mesh_3(c3t3, domain, new_criteria, params::manifold());

        // Output
        medit_file.open(output_refinement);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
        medit_file.close();
    }
}


#endif //POISSONRECONCGAL_TET_SURFACE_MESH_WITHOUT_FEATURE_H
