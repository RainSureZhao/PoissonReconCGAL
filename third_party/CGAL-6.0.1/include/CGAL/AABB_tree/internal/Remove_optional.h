// Copyright (c) 2024 INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v6.0.1/AABB_tree/include/CGAL/AABB_tree/internal/Remove_optional.h $
// $Id: include/CGAL/AABB_tree/internal/Remove_optional.h 50cfbde3b84 $
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s) : Stéphane Tayeb, Pierre Alliez, Camille Wormser
//

#ifndef CGAL_REMOVE_OPTIONAL_H_
#define CGAL_REMOVE_OPTIONAL_H_

#include <CGAL/license/AABB_tree.h>

#include <optional>

namespace CGAL {
namespace internal {

template <class T>
struct Remove_optional { typedef T type; };

template <class T>
struct Remove_optional< ::std::optional<T> > { typedef T type; };

}  // end namespace internal
}  // end namespace CGAL

#endif