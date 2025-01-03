// Copyright (c) 2006  GeometryFactory (France). All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v6.0.1/Surface_mesh_simplification/include/CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h $
// $Id: include/CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h 50cfbde3b84 $
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Fernando Cacciola <fernando.cacciola@geometryfactory.com>
//
#ifndef CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_MIDPOINT_PLACEMENT_H
#define CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_MIDPOINT_PLACEMENT_H

#include <CGAL/license/Surface_mesh_simplification.h>


#include <CGAL/Surface_mesh_simplification/internal/Common.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_profile.h>

namespace CGAL {
namespace Surface_mesh_simplification {

template<class TM_>
class Midpoint_placement
{
public:
  typedef TM_                                                     TM;

  Midpoint_placement() {}

  template <typename Profile>
  std::optional<typename Profile::Point> operator()(const Profile& profile) const
  {
    typedef std::optional<typename Profile::Point>              result_type;
    return result_type(profile.geom_traits().construct_midpoint_3_object()(profile.p0(), profile.p1()));
  }
};

} // namespace Surface_mesh_simplification
} // namespace CGAL

#endif // CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_MIDPOINT_PLACEMENT_H
