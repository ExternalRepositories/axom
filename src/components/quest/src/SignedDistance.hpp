/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and further
 * review from Lawrence Livermore National Laboratory.
 */

/*!
 *******************************************************************************
 * \file SignedDistance.hpp
 *
 * \date Feb 15, 2016
 * \author George Zagaris (zagaris2@llnl.gov)
 *******************************************************************************
 */

#ifndef SIGNEDDISTANCE_HPP_
#define SIGNEDDISTANCE_HPP_

// ATK includes
#include "common/ATKMacros.hpp"
#include "common/CommonTypes.hpp"

#include "quest/BoundingBox.hpp"
#include "quest/BucketTree.hpp"
#include "quest/Field.hpp"
#include "quest/FieldData.hpp"
#include "quest/FieldVariable.hpp"
#include "quest/Mesh.hpp"
#include "quest/Orientation.hpp"
#include "quest/Point.hpp"
#include "quest/Triangle.hpp"

// C/C++ includes
#include <cmath> // for std::sqrt()

namespace quest {

template < int NDIMS >
class SignedDistance
{
public:
  typedef Point< double,NDIMS > PointType;
  typedef Triangle< double,NDIMS > TriangleType;
  typedef BoundingBox< double,NDIMS > BoxType;
  typedef BucketTree< int,NDIMS > BucketTreeType;

public:

  /*!
   *****************************************************************************
   * \brief Creates a SignedDistance instance for queries on the given mesh.
   * \param [in] surfaceMesh user-supplied surface mesh.
   * \param [in] maxObjects max number of objects for spatial decomposition.
   * \param [in] maxLevel max levels for spatial decomposition (optional).
   * \note Default maxLevel is 5 if not specified.
   * \pre surfaceMesh != ATK_NULLPTR
   *****************************************************************************
   */
  SignedDistance( meshtk::Mesh* surfaceMesh, int maxObjects, int maxLevels=5 );

  /*!
   *****************************************************************************
   * \brief Destructor.
   *****************************************************************************
   */
  ~SignedDistance();

  /*!
   *****************************************************************************
   * \brief Computes the distance of the given point to the surface mesh.
   * \param [in] queryPnt user-supplied point.
   * \return minDist the signed minimum distance to the surface mesh.
   *****************************************************************************
   */
  double computeDistance( const PointType& queryPnt ) const;

  /*!
   *****************************************************************************
   * \brief Returns a const reference to the underlying bucket tree.
   * \return ptr pointer to the underlying bucket tree
   * \post ptr != ATK_NULLPTR
   *****************************************************************************
   */
  const BucketTreeType* getBucketTree( ) const { return m_bucketTree; };

private:

  /*!
   *****************************************************************************
   * \brief Updates the minimum squared distance of the point to the given cell.
   * \param [in] pt the query point.
   * \param [in] icell the cell on the surface mesh.
   * \param [in/out] minSqDist the minimum squared distance.
   * \param [in/out] sign the sign of the squared distance.
   * \pre m_surfaceMesh != ATK_NULLPTR.
   * \pre icell >= 0 && icell < m_surfaceMesh->getMeshNumberOfCells().
   *****************************************************************************
   */
  void updateSignedDistance( const PointType& pt, int icell,
                             double& minSqDist, double& sign ) const;

  /*!
   *****************************************************************************
   * \brief Computes the bounding box of the given cell on the surface mesh.
   * \param [in] icell the index of the cell on the surface mesh.
   * \return box bounding box of the cell.
   * \pre m_surfaceMesh != ATK_NULLPTR
   * \pre icell >= 0 && icell < m_surfaceMesh->getMeshNumberOfCells()
   *****************************************************************************
   */
  BoxType getCellBoundingBox( int icell );

  /*!
   *****************************************************************************
   * \brief Default constructor. Does nothing.
   * \note Made private to prevent its use from the calling application.
   *****************************************************************************
   */
  SignedDistance(): m_surfaceMesh(ATK_NULLPTR), m_bucketTree(ATK_NULLPTR) { };

private:

  meshtk::Mesh* m_surfaceMesh;   /*!< User-supplied surface mesh. */
  BucketTreeType* m_bucketTree;  /*!< Spatial acceleration data-structure. */

  DISABLE_COPY_AND_ASSIGNMENT( SignedDistance );

};

} /* namespace quest */

//------------------------------------------------------------------------------
//           SignedDistance Implementation
//------------------------------------------------------------------------------
namespace quest
{

//------------------------------------------------------------------------------
template < int NDIMS >
SignedDistance< NDIMS >::SignedDistance(
        meshtk::Mesh* surfaceMesh, int maxObjects, int maxLevels )
{
  // Sanity checks
  SLIC_ASSERT( surfaceMesh != ATK_NULLPTR );
  SLIC_ASSERT( maxLevels >= 1 );

  m_surfaceMesh    = surfaceMesh;
  const int ncells = m_surfaceMesh->getMeshNumberOfCells();

  // Initialize BucketTree with the surface elements.
  m_bucketTree  = new BucketTreeType( ncells, maxLevels );

  for ( int icell=0; icell < ncells; ++icell ) {
      m_bucketTree->insert( this->getCellBoundingBox( icell ), icell );
  } // END for all cells

  // Build bounding volume hierarchy
  m_bucketTree->build( maxObjects );
}

//------------------------------------------------------------------------------
template < int NDIMS >
SignedDistance< NDIMS >::~SignedDistance( )
{
  delete m_bucketTree;
  m_bucketTree = ATK_NULLPTR;
}

//------------------------------------------------------------------------------
template < int NDIMS >
inline double
SignedDistance< NDIMS >::computeDistance( const PointType& pt ) const
{
  SLIC_ASSERT( m_surfaceMesh != ATK_NULLPTR );
  SLIC_ASSERT( m_bucketTree != ATK_NULLPTR );

  double minSqDist = std::numeric_limits< double >::max();
  double sign      = 0.0;

  std::vector< int > candidate_buckets;
  m_bucketTree->find( pt, candidate_buckets );

  const int nbuckets = candidate_buckets.size();
  for ( int ibucket=0; ibucket < nbuckets; ++ibucket ) {

     const int bucketIdx  = candidate_buckets[ ibucket ];
     const int numObjects = m_bucketTree->getBucketNumObjects( bucketIdx );
     const int* objIdList = m_bucketTree->getBucketObjectArray( bucketIdx );

     for ( int iobject=0; iobject < numObjects; ++iobject ) {

        const int objIdx  = objIdList[ iobject ];
        const int cellIdx = m_bucketTree->getObjectData( objIdx );
        SLIC_ASSERT( (cellIdx >= 0) &&
                     (cellIdx < m_surfaceMesh->getMeshNumberOfCells()) );

        this->updateSignedDistance( pt, cellIdx, minSqDist, sign );
     } // END for all iobjects

  } // END for all buckets

  return ( sign*std::sqrt(minSqDist) );
}

//------------------------------------------------------------------------------
template < int NDIMS >
inline void
SignedDistance< NDIMS >::updateSignedDistance(
        const PointType& pt, int icell, double& minSqDist, double& sign ) const
{
  // Sanity checks
  SLIC_ASSERT( m_surfaceMesh != ATK_NULLPTR );
  SLIC_ASSERT( icell >= 0 && icell < m_surfaceMesh->getMeshNumberOfCells() );

  // Get the cell type, for now we support linear triangle,quad in 3-D and
  // line segments in 2-D.
  const int cellType = m_surfaceMesh->getMeshCellType( icell );

  // TODO: for now we assume a triangle mesh, the squared_distance() must be
  // updated to support, quad, etc., punting it for now...
  SLIC_ASSERT( cellType==meshtk::LINEAR_TRIANGLE );
//  const int nnodes = meshtk::cell::num_nodes[ cellType ];

  // Get the cell node IDs that make up the cell
  int cellIds[3];
  m_surfaceMesh->getMeshCell( icell, cellIds );

  PointType a, b, c;
  m_surfaceMesh->getMeshNode( cellIds[0], a.data() );
  m_surfaceMesh->getMeshNode( cellIds[1], b.data() );
  m_surfaceMesh->getMeshNode( cellIds[2], c.data() );

  TriangleType surfTri( a, b, c );
  const double sqDist = quest::squared_distance( pt, surfTri );
  if ( sqDist < minSqDist ) {

    minSqDist      = sqDist;
    bool onNegSide = quest::orientation( pt, surfTri )==quest::ON_NEGATIVE_SIDE;
    sign = ( onNegSide )? -1.0 : 1.0;

  } // END if

}

//------------------------------------------------------------------------------
template < int NDIMS >
inline BoundingBox<double,NDIMS>
SignedDistance< NDIMS >::getCellBoundingBox( int icell )
{
  // Sanity checks
  SLIC_ASSERT( m_surfaceMesh != ATK_NULLPTR );
  SLIC_ASSERT( icell >= 0 && icell < m_surfaceMesh->getMeshNumberOfCells() );

  // Get the cell type, for now we support linear triangle,quad in 3-D and
  // line segments in 2-D.
  const int cellType = m_surfaceMesh->getMeshCellType( icell );
  SLIC_ASSERT( cellType==meshtk::LINEAR_TRIANGLE ||
               cellType==meshtk::LINEAR_QUAD ||
               cellType==meshtk::LINE );
  const int nnodes = meshtk::cell::num_nodes[ cellType ];

  // Get the cell node IDs that make up the cell
  int* cellIds = new int[ nnodes ];
  m_surfaceMesh->getMeshCell( icell, cellIds );

  // compute the cell's bounding box
  BoxType bb;
  PointType pt;

  for ( int i=0; i < nnodes; ++i ) {

     m_surfaceMesh->getMeshNode( cellIds[ i ], pt.data() );
     bb.addPoint( pt );

  } // END for all cell nodes

  // clean up all dynamically allocated memory
  delete [] cellIds;

  return ( bb );
}

} /* namespace quest */
#endif /* SIGNEDDISTANCE_HPP_ */
