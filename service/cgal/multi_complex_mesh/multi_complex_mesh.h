//
// Created by RainSure on 24-10-17.
//

#ifndef POISSONRECONCGAL_MULTI_COMPLEX_MESH_H
#define POISSONRECONCGAL_MULTI_COMPLEX_MESH_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Polyhedral_complex_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>

#include <cstdlib>
#include <cassert>

#include <vector>
#include <cstring>
#include <fstream>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Mesh_polyhedron_3<K>::type Polyhedron;
typedef CGAL::Polyhedral_complex_mesh_domain_3<K> Mesh_domain;


#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain,CGAL::Default,Concurrency_tag>::type Tr;

typedef CGAL::Mesh_complex_3_in_triangulation_3<
        Tr,Mesh_domain::Corner_index,Mesh_domain::Curve_index> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

namespace params = CGAL::parameters;

namespace cdt {
    /**
    * @brief 从多个表面网格生成四面体网格，表面网格必须要是三角网格
    * @param meshs_input 输入的表面网格
     * @param incident_subdomains  子域两两配对的数组
    * @param output 输出的四面体网格 xxx.mesh
    */
    void tetrahedralization_by_surface_meshs(const std::vector<std::string>& meshs_input, const std::vector<std::pair<int, int>>& incident_subdomains, const std::string& output) {
        size_t nb_patches = meshs_input.size();
        std::vector<Polyhedron> patches(nb_patches);
        assert(meshs_input.size() == incident_subdomains.size());
        for(std::size_t i = 0; i < nb_patches; ++i) {
            std::ifstream input(CGAL::data_file_path(meshs_input[i]));
            if(!(input >> patches[i])) {
                std::cerr << "Error reading " << CGAL::data_file_path(meshs_input[i]) << " as a polyhedron!\n";
                return;
            }
        }
        // Create domain
        Mesh_domain domain(patches.begin(), patches.end(),
                           begin(incident_subdomains), end(incident_subdomains));

        domain.detect_features(); //includes detection of borders

        // Mesh criteria
        Mesh_criteria criteria(params::edge_size(8).
                facet_angle(25).facet_size(8).facet_distance(0.2).
                cell_radius_edge_ratio(3).cell_size(10));

        // Mesh generation
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);

        // Output
        std::ofstream medit_file(output);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
        medit_file.close();
    }
}


#endif //POISSONRECONCGAL_MULTI_COMPLEX_MESH_H
