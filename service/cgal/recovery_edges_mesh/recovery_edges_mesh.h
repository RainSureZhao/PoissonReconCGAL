//
// Created by RainSure on 24-10-17.
//

#ifndef POISSONRECONCGAL_RECOVERY_EDGES_MESH_H
#define POISSONRECONCGAL_RECOVERY_EDGES_MESH_H

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

#include "utils/read_polylines.h"

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
typedef CGAL::Mesh_triangulation_3<Mesh_domain, K>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr>     Mesh_criteria;
typedef Mesh_criteria::Edge_criteria  Edge_criteria;
typedef Mesh_criteria::Facet_criteria Facet_criteria;
typedef Mesh_criteria::Cell_criteria  Cell_criteria;

// Function
typedef K::FT FT;
typedef K::Point_3 Point;

FT sphere_centered_at_111 (const Point& p)
{
    const FT dx=p.x()-1;
    const FT dy=p.y()-1;
    const FT dz=p.z()-1;

    return dx*dx+dy*dy+dz*dz-1;
}

namespace params = CGAL::parameters;

namespace cdt {
    /**
     * @brief 从表面网格中恢复边界
     * @param meshs
     * @param incident_subdomains
     * @param edges
     * @param output
     */
    void recovery_edges_from_surface(const std::vector<std::string>& meshs, const std::vector<std::pair<int, int>>& incident_subdomains, const std::string& edges, const std::string& output) {
        size_t nb_patches = meshs.size();
        std::vector<Polyhedron> patches(nb_patches);
        for(std::size_t i = 0; i < nb_patches; ++i) {
            std::ifstream input(CGAL::data_file_path(meshs[i]));
            if(!(input >> patches[i])) {
                std::cerr << "Error reading " << CGAL::data_file_path(meshs[i]) << " as a polyhedron!\n";
                return;
            }
        }

        // Create domain
        Mesh_domain domain(patches.begin(), patches.end(),
                           begin(incident_subdomains), end(incident_subdomains));

        domain.detect_features(); //includes detection of borders

        std::vector<std::vector<Point> > featured_curves;
        if(!edges.empty()) {
            if (!read_polylines(edges, featured_curves))
            { // see file "read_polylines.h"
                std::cerr << "Error: Cannot read file " << edges << std::endl;
                return;
            }
        }

        // Add features for protection
        domain.add_features(featured_curves.begin(), featured_curves.end());

        // Criteria
        Edge_criteria edge_criteria(8);
        Facet_criteria facet_criteria(30, 8, 5); // angle, size, approximation
        Cell_criteria cell_criteria(2, 1); // radius-edge ratio, size
        Mesh_criteria criteria(edge_criteria, facet_criteria, cell_criteria);

        // Mesh generation (without optimization)
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria,
                                            params::no_perturb().no_exude());

        // Output
        std::ofstream medit_file(output);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
        medit_file.close();
    }

    /**
     * @brief 支持设置各种参数的从表面网格中恢复边界
     * @param meshs
     * @param incident_subdomains
     * @param edges
     * @param output
     * @param edge_bound
     * @param facet_angle_min_bound
     * @param facet_size_bound
     * @param facet_approximation_bound
     * @param cell_radius_edge_bound
     * @param cell_size_bound
     */
    void recovery_edges_from_surface_with_criteria(const std::vector<std::string>& meshs, const std::vector<std::pair<int, int>>& incident_subdomains,
                                                   const std::string& edges, const std::string& output, double edge_bound, double facet_angle_min_bound,
                                                   double facet_size_bound, double facet_approximation_bound, double cell_radius_edge_bound, double cell_size_bound) {
        size_t nb_patches = meshs.size();
        std::vector<Polyhedron> patches(nb_patches);
        for(std::size_t i = 0; i < nb_patches; ++i) {
            std::ifstream input(CGAL::data_file_path(meshs[i]));
            if(!(input >> patches[i])) {
                std::cerr << "Error reading " << CGAL::data_file_path(meshs[i]) << " as a polyhedron!\n";
                return;
            }
        }

        // Create domain
        Mesh_domain domain(patches.begin(), patches.end(),
                           begin(incident_subdomains), end(incident_subdomains));

        domain.detect_features(); //includes detection of borders

        std::vector<std::vector<Point> > featured_curves;
        if(!edges.empty()) {
            if (!read_polylines(edges, featured_curves))
            { // see file "read_polylines.h"
                std::cerr << "Error: Cannot read file " << edges << std::endl;
                return;
            }
        }

        // Add features for protection
        domain.add_features(featured_curves.begin(), featured_curves.end());

        // Criteria
        Edge_criteria edge_criteria(edge_bound);
        Facet_criteria facet_criteria(facet_angle_min_bound, facet_size_bound, facet_approximation_bound); // angle, size, approximation
        Cell_criteria cell_criteria(cell_radius_edge_bound, cell_size_bound); // radius-edge ratio, size
        Mesh_criteria criteria(edge_criteria, facet_criteria, cell_criteria);

        // Mesh generation (without optimization)
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria,
                                            params::no_perturb().no_exude());

        // Output
        std::ofstream medit_file(output);
        CGAL::IO::write_MEDIT(medit_file, c3t3);
        medit_file.close();
    }
}


#endif //POISSONRECONCGAL_RECOVERY_EDGES_MESH_H
