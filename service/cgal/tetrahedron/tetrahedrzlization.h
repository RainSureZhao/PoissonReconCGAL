//
// Created by RainSure on 24-12-21.
//

#ifndef POISSONRECONCGAL_TETRAHEDRZLIZATION_H
#define POISSONRECONCGAL_TETRAHEDRZLIZATION_H
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Polyhedral_complex_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>


namespace service::cgal::tetrahedron {
    namespace params = CGAL::parameters;
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

    /**
     * @brief 从多个表面网格生成四面体网格，表面网格必须要是三角网格，同时支持恢复特征边
     * @param patches
     * @param featuredCurves
     * @param incident_subdomains
     * @param output
     * @param edge_bound
     * @param facet_angle_min_bound
     * @param facet_size_bound
     * @param facet_approximation_bound
     * @param cell_radius_edge_bound
     * @param cell_size_bound
     */
    void Tetrahedralization(const std::vector<Polyhedron>& patches, const std::vector<std::vector<Point>>& featuredCurves,
                            const std::vector<std::pair<int, int>>& incident_subdomains, const std::string& output, double edge_bound, double facet_angle_min_bound,
                            double facet_size_bound, double facet_approximation_bound, double cell_radius_edge_bound, double cell_size_bound) {
        assert(incident_subdomains.size() == patches.size());
        // Create domain
        Mesh_domain domain(patches.begin(), patches.end(),
                           begin(incident_subdomains), end(incident_subdomains));

        // domain.detect_features(); //includes detection of borders

        // Add features for protection
        domain.add_features(featuredCurves.begin(), featuredCurves.end());

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

#endif //POISSONRECONCGAL_TETRAHEDRZLIZATION_H
