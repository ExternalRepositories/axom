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

#ifndef TETRAHEDRON_HPP_
#define TETRAHEDRON_HPP_

#include "axom_utils/Determinants.hpp" // For numerics::determinant()
#include "axom_utils/Utilities.hpp"

#include "primal/Point.hpp"
#include "primal/Vector.hpp"

#include "slic/slic.hpp"


namespace axom
{
namespace primal
{

template < typename T,int NDIMS >
class Tetrahedron
{
public:
  typedef Point< T,NDIMS >  PointType;
  typedef Vector< T,NDIMS > VectorType;

  enum
  {
    NUM_TET_VERTS = 4
  };

public:

  /*!
   * \brief Default constructor. Creates a degenerate tetrahedron.
   */
  Tetrahedron() { }

  /*!
   * \brief Custom Constructor. Creates a tetrahedron from the 4 points A,B,C,D.
   * \param [in] A point instance corresponding to vertex A of the tetrahedron.
   * \param [in] B point instance corresponding to vertex B of the tetrahedron.
   * \param [in] C point instance corresponding to vertex C of the tetrahedron.
   * \param [in] D point instance corresponding to vertex D of the tetrahedron.
   */
  Tetrahedron( const PointType& A,
               const PointType& B,
               const PointType& C,
               const PointType& D ){
    m_points[0] = A;
    m_points[1] = B;
    m_points[2] = C;
    m_points[3] = D;
  }

  /*!
   * \brief Destructor
   */
  ~Tetrahedron() { }

  /*!
   * \brief Index operator to get the i^th vertex
   * \param idx The index of the desired vertex
   * \pre idx is 0, 1, 2, or 3
   */
  PointType& operator[](int idx)
  {
    SLIC_ASSERT(idx >=0 && idx < NUM_TET_VERTS);
    return m_points[ idx ];
  }

  /*!
   * \brief Index operator to get the i^th vertex
   * \param idx The index of the desired vertex
   * \pre idx is 0, 1, 2, or 3
   */
  const PointType& operator[](int idx) const
  {
    SLIC_ASSERT(idx >=0 && idx < NUM_TET_VERTS);
    return m_points[ idx ];
  }


  /*!
   * \brief Returns the barycentric coordinates of a point within a tetrahedron
   * \return The barycentric coordinates of the tetrahedron
   * \post The barycentric coordinates sum to 1.
   */
  Point< double,4 > physToBarycentric(const PointType& p) const
  {
    Point< double,4 > bary;

    const PointType& p0 = m_points[0];
    const PointType& p1 = m_points[1];
    const PointType& p2 = m_points[2];
    const PointType& p3 = m_points[3];

    const double det0 = axom::numerics::determinant(
      p0[0], p0[1], p0[2], 1.0,
      p1[0], p1[1], p1[2], 1.0,
      p2[0], p2[1], p2[2], 1.0,
      p3[0], p3[1], p3[2], 1.0 );

    SLIC_CHECK_MSG(
      !axom::utilities::isNearlyEqual(det0,0.),
      "Attempting to find barycentric coordinates of degenerate tetrahedron");

    const double detScale = 1. / det0;


    const double det1 = axom::numerics::determinant(
      p[0],  p[1],  p[2], 1.0,
      p1[0], p1[1], p1[2], 1.0,
      p2[0], p2[1], p2[2], 1.0,
      p3[0], p3[1], p3[2], 1.0 );
    const double det2 = axom::numerics::determinant(
      p0[0], p0[1], p0[2], 1.0,
      p[0],  p[1],  p[2], 1.0,
      p2[0], p2[1], p2[2], 1.0,
      p3[0], p3[1], p3[2], 1.0 );
    const double det3 = axom::numerics::determinant(
      p0[0], p0[1], p0[2], 1.0,
      p1[0], p1[1], p1[2], 1.0,
      p[0],  p[1],  p[2], 1.0,
      p3[0], p3[1], p3[2], 1.0 );
    const double det4 = axom::numerics::determinant(
      p0[0], p0[1], p0[2], 1.0,
      p1[0], p1[1], p1[2], 1.0,
      p2[0], p2[1], p2[2], 1.0,
      p[0],  p[1],  p[2], 1.0 );

    bary[0] = det1 * detScale;
    bary[1] = det2 * detScale;
    bary[2] = det3 * detScale;
    bary[3] = det4 * detScale;

    return bary;

  }

  /*!
   * \brief Simple formatted print of a tetrahedron instance
   * \param os The output stream to write to
   * \return A reference to the modified ostream
   */
  std::ostream& print(std::ostream& os) const
  {
    os <<"{"
       << m_points[0] <<" "
       << m_points[1] <<" "
       << m_points[2] <<" "
       << m_points[3] <<"}";

    return os;
  }

private:
  PointType m_points[ 4 ];

};


//------------------------------------------------------------------------------
/// Free functions implementing Tetrahedron's operators
//------------------------------------------------------------------------------
template < typename T, int NDIMS >
std::ostream& operator<<(std::ostream & os, const Tetrahedron< T,NDIMS > & tet)
{
  tet.print(os);
  return os;
}


} /* namespace primal */
} /* namespace axom */

#endif /* TETRAHEDRON_HPP_ */
