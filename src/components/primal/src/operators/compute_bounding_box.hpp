/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright (c) 2017, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * LLNL-CODE-741217
 *
 * All rights reserved.
 *
 * This file is part of Axom.
 *
 * For details about use and distribution, please read axom/LICENSE.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*!
 * \file compute_bounding_box.hpp
 *
 * \brief Consists of functions to create bounding boxes.
 */

#ifndef COMPUTE_BOUNDING_BOX_HPP_
#define COMPUTE_BOUNDING_BOX_HPP_

#include "primal/NumericArray.hpp" // for numeric arrays
#include "axom_utils/Matrix.hpp" // for Matrix
#include "axom_utils/eigen_solve.hpp" // for eigen_solve
#include "primal/Point.hpp"
#include "primal/Vector.hpp"
#include "primal/OrientedBoundingBox.hpp"

namespace axom
{
namespace primal
{

/*!
 * \brief Creates a bounding box which contains the collection of passed in
 * points.
 *
 * Call srand() to initialize the random number generator before using this
 * function.
 *
 * \param [in] pts array of points
 * \param [in] n number of points
 * \note if n <= 0, invokes default constructor
 */
template < typename T, int NDIMS >
OrientedBoundingBox< T, NDIMS > compute_oriented_bounding_box(
  const Point< T, NDIMS >* pts, int n)
{
  return OrientedBoundingBox< T, NDIMS >(pts, n);
}

/*!
 * \brief Creates an oriented bounding box which contains the passed in OBBs.
 *
 * Call srand() to initialize the random number generator before using this
 * function.
 *
 * \param [in] l left obb
 * \param [in] r right obb
 */
template < typename T, int NDIMS >
OrientedBoundingBox< T, NDIMS > merge_boxes( const OrientedBoundingBox< T, NDIMS >
                                             &l, const OrientedBoundingBox< T,
                                                                            NDIMS > &r)
{

  // TODO: See if this initial check can be improved so it's not so costly in
  // cases where it doesn't end up helping
  if (l.contains(r))
  {
    return l;
  }
  if (r.contains(l))
  {
    return r;
  }

  std::vector< Point< T, NDIMS > > lv = l.vertices();
  std::vector< Point< T, NDIMS > > rv = r.vertices();

  const int size = (1 << NDIMS);

  Point< T, NDIMS > pts[2*size];

  for (int i = 0 ; i < size ; i++)
  {
    pts[i] = lv[i];
    pts[i + size] = rv[i];
  }

  return compute_oriented_bounding_box< T, NDIMS >(pts, 2*size);
}

/*!
 * \brief Constructor. Creates a bounding box which contains the passed in
 * bounding boxes.
 *
 * \param [in] l left bb
 * \param [in] r right bb
 */
template < typename T, int NDIMS >
BoundingBox< T, NDIMS > merge_boxes( const BoundingBox< T, NDIMS >
                                     &l, const BoundingBox< T, NDIMS > &r)
{
  BoundingBox< T, NDIMS > res(l);
  res.addBox(r);
  return res;
}

}  /* namespace axom */
}  /* namespace primal */

#endif  /* COMPUTE_BOUNDING_BOX_HPP_ */
