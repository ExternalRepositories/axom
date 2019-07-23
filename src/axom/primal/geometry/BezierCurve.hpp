// Copyright (c) 2017-2019, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

/*!
 * \file BezierCurve.hpp
 *
 * \brief A BezierCurve primitive
 */

#ifndef PRIMAL_BEZIERCURVE_HPP_
#define PRIMAL_BEZIERCURVE_HPP_

#include "axom/slic.hpp"

#include "axom/primal/geometry/NumericArray.hpp"
#include "axom/primal/geometry/Point.hpp"
#include "axom/primal/geometry/Vector.hpp"
#include "axom/primal/geometry/Segment.hpp"
#include "axom/primal/geometry/BoundingBox.hpp"
#include "axom/primal/geometry/OrientedBoundingBox.hpp"

#include "axom/primal/operators/squared_distance.hpp"

#include "fmt/fmt.hpp"
#include <vector>
#include <ostream>

namespace axom
{
namespace primal
{

// Forward declare the templated classes and operator functions
template < typename T, int NDIMS >
class BezierCurve;

/*! \brief Overloaded output operator for Bezier Curves*/
template < typename T,int NDIMS >
std::ostream& operator<<(std::ostream & os,
                         const BezierCurve< T,NDIMS > & bCurve);

/*!
 * \class BezierCurve
 *
 * \brief Represents a Bezier curve defined by an array of control points
 * \tparam T the coordinate type, e.g., double, float, etc.
 * \tparam NDIMS the number of dimensions
 *
 * \note The order of a Bezier curve with N+1 control points is N
 * \note The control points should be ordered from t=0 to t=1
 */

template < typename T,int NDIMS >
class BezierCurve
{
public:
  using PointType = Point< T,NDIMS >;
  using VectorType = Vector< T,NDIMS >;
  using NumArrayType = NumericArray< T,NDIMS >;
  using SegmentType = Segment< T, NDIMS >;
  using CoordsVec = std::vector< PointType >;
  using BoundingBox = BoundingBox< T, NDIMS >;
  using OrientedBoundingBox = OrientedBoundingBox< T, NDIMS >;

public:
  /*! Default constructor for an empty Bezier Curve*/
  BezierCurve() = default;

  /*!
   * \brief Constructor for an empty Bezier Curve that reserves space for
   *  the given order of the curve
   *
   * \param [in] order the order of the resulting Bezier curve
   * \pre order is not negative
   */
  BezierCurve(int ord)
  {
    SLIC_ASSERT(ord >= 0);
    m_controlPoints.reserve(ord+1);
    m_controlPoints.resize(ord+1);
  }

  /*!
   * \brief Constructor for a Bezier Curve from a list of Points
   * \verbatim {x_0, x_1, x_2, x_3,
   *            y_0, y_1, y_2, y_3,
   *            z_0, z_1, z_2, z_3}
   *
   * \param [in] pts an array with (n+1)*NDIMS entries, ordered by coordinate
   * then by control point order
   * \param [in] ord number of control points minus 1 for which to reserve
   * control point space
   * \pre order is not negative
   */
  BezierCurve(T* pts, int ord)
  {
    if ( ord <= 0 )
    {
      clear();
    }
    // sanity check
    SLIC_ASSERT(pts != nullptr);

    m_controlPoints.reserve(ord+1);

    T tempar[NDIMS];
    for ( int p = 0 ; p <= ord ; p++)
    {
      for ( int j = 0 ; j < NDIMS ; j++)
      {
        tempar[j]=pts[j*(ord+1)+p];
      }
      this->addControlPoint(tempar);
    }
  }

  /*!
   * \brief Constructor for a Bezier Curve from an array of coordinates
   *
   * \param [in] pts a vector with ord+1 points in it
   * \param [in] ord number of control points minus 1 for which to reserve
   * control point space
   * \pre order is not negative
   *
   */

  BezierCurve(PointType* pts, int ord)
  {
    if (ord <= 0)
    {
      clear();
    }
    // sanity check
    SLIC_ASSERT(pts != nullptr);

    m_controlPoints.reserve(ord+1);

    for (int p = 0 ; p <= ord ; ++p)
    {
      this->addControlPoint(pts[p]);
    }
  }

  /*! Sets the order of the Bezier Curve*/
  void setOrder( int ord)
  { m_controlPoints.resize(ord+1); }

  /*! Returns the order of the Bezier Curve*/
  int getOrder() const
  { return m_controlPoints.size()-1; }

  /*! Appends a control point to the list of control points*/
  void addControlPoint(const PointType& pt)
  {
    m_controlPoints.push_back(pt);
  }

  /*! Clears the list of control points*/
  void clear()
  {
    m_controlPoints.clear();
  }

  /*! Retrieves the control point at index \a idx */
  PointType& operator[](int idx) { return m_controlPoints[idx]; }
  /*! Retrieves the control point at index \a idx */
  const PointType& operator[](int idx) const { return m_controlPoints[idx]; }

  /* Checks equality of two Bezier Curve */
  friend inline bool operator==(const BezierCurve< T, NDIMS>& lhs,
                                const BezierCurve< T, NDIMS>& rhs)
  {
    return lhs.m_controlPoints == rhs.m_controlPoints;
  }

  friend inline bool operator!=(const BezierCurve< T, NDIMS>& lhs,
                                const BezierCurve< T, NDIMS>& rhs)
  {
    return !(lhs == rhs);
  }

  /*! Returns a copy of the Bezier curve's control points */
  CoordsVec getControlPoints() const
  {
    return m_controlPoints;
  }

  /*! Returns an axis-aligned bounding box containing the Bezier curve */
  BoundingBox boundingBox() const
  {
    return BoundingBox(m_controlPoints.data(), m_controlPoints.size());
  }

  /*! Returns an oriented bounding box containing the Bezier curve */
  OrientedBoundingBox orientedBoundingBox() const
  {
    return OrientedBoundingBox(m_controlPoints.data(), m_controlPoints.size());
  }

  /*!
   * \brief Evaluates a Bezier curve at a particular parameter value \a t
   *
   * \param [in] t parameter value at which to evaluate
   * \return p the value of the Bezier curve at t
   *
   * \note We typically evaluate the curve at \a t between 0 and 1
   */

  PointType evaluate(T t) const
  {
    PointType ptval;

    const int ord = getOrder();
    std::vector<T> dCarray(ord+1);

    // Run de Casteljau algorithm on each dimension
    for ( int i=0 ; i < NDIMS ; ++i)
    {
      for ( int p=0 ; p <= ord ; ++p)
      {
        dCarray[p] = m_controlPoints[p][i];
      }

      for ( int p=1 ; p <= ord ; ++p)
      {
        const int end = ord-p;
        for ( int k=0 ; k <= end ; ++k)
        {
          dCarray[k]=(1-t)*dCarray[k] + t*dCarray[k+1];
        }
      }
      ptval[i]=dCarray[0];
    }

    return ptval;
  }

  /*!
   * \brief Splits a Bezier curve into two Bezier curves at particular parameter
   * value between 0 and 1
   *
   * \param [in] t parameter value between 0 and 1 at which to evaluate
   * \param [out] c1, c2 Bezier curves that split the original
   */
  void split(T t, BezierCurve& c1, BezierCurve& c2) const
  {
    int ord = getOrder();
    SLIC_ASSERT(ord >= 0);

    // Note: the second curve's control points are computed inline
    //       as we find the first curve's control points
    c2 = *this;

    c1.setOrder(ord);
    c1[0] = c2[0];

    // Run de Casteljau algorithm
    // After each iteration, save the first control point into c1
    for ( int p=1 ; p <= ord ; ++p)
    {
      const int end = ord-p;
      for(int k=0 ; k<= end ; ++k)
      {
        PointType& pt1 = c2[k];
        const PointType& pt2 = c2[k+1];
        for(int i=0 ; i< NDIMS ; ++i)
        {
          pt1[i] = (1-t)*pt1[i] + t*pt2[i];
        }
      }
      c1[p] = c2[0];
    }

    return;
  }

  /*!
   * \brief Predicate to check if the Bezier curve is approximately linear
   *
   * This function checks if the internal control points of the BezierCurve
   * are approximately on the line defined by its two endpoints
   *
   * \param [in] tol Threshold for sum of squared distances
   * \return True if c1 is near-linear
   */

  bool isLinear(double tol = 1E-8) const
  {
    const int ord = getOrder();
    if(ord <= 1)
    {
      return true;
    }

    SegmentType seg(m_controlPoints[0], m_controlPoints[ord]);
    double sqDist =0.0;
    for (int p=1 ; p<ord && sqDist < tol ; ++p) // check interior control points
    {
      sqDist += squared_distance(m_controlPoints[p],seg);
    }
    return (sqDist < tol);
  }

  /*!
   * \brief Simple formatted print of a Bezier Curve instance
   *
   * \param os The output stream to write to
   * \return A reference to the modified ostream
   */

  std::ostream& print(std::ostream& os) const
  {
    const int ord = getOrder();

    os <<"{" << ord <<"-degree Bezier Curve:";
    for (int p=0 ; p< ord ; ++p)
    {
      os << m_controlPoints[p] << ",";
    }
    os<<m_controlPoints[ord];
    os<< "}";

    return os;
  }

private:
  CoordsVec m_controlPoints;
};

//------------------------------------------------------------------------------
/// Free functions implementing BezierCurve's operators
//------------------------------------------------------------------------------
template < typename T, int NDIMS >
std::ostream& operator<<(std::ostream & os,
                         const BezierCurve< T,NDIMS > & bCurve)
{
  bCurve.print(os);
  return os;
}

} // namespace primal
} // namespace axom

#endif // PRIMAL_BEZIERCURVE_HPP_
