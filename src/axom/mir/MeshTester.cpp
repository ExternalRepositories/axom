// Copyright (c) 2017-2019, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "MeshTester.hpp"

namespace numerics = axom::numerics;
namespace slam = axom::slam;

namespace axom
{
namespace mir
{

//--------------------------------------------------------------------------------

MIRMesh MeshTester::initTestCaseOne()
{
   mir::CellTopologyData topoData;
   mir::CellMapData mapData;
   mir::CellData cellData;
   VolumeFractions volFracs;

  int numElements = 9;
  int numVertices = 16;
  mir::VertSet  verts(numVertices);  // Construct a vertex set with 16 vertices
  mir::ElemSet  elems(numElements);   // Construct an element set with 9 elements

  // Create the mesh connectivity information
  topoData.m_evInds = {
      0,4,5,1,     // elem 0, card 4, start 0
      1,5,6,2,     // elem 1, card 4, start 4
      2,6,7,3,     // elem 2, card 4, start 8
      4,8,9,5,     // elem 3, card 4, start 12
      5,9,10,6,    // elem 4, card 4, start 16
      6,10,11,7,   // elem 5, card 4, start 20
      8,12,13,9,   // elem 6, card 4, start 24
      9,13,14,10,  // elem 7, card 4, start 28
      10,14,15,11  // elem 8, card 4, start 32, end 36
    };

  topoData.m_evBegins = {
      0,4,8,12,16,20,24,28,32,36
    };
  topoData.m_veInds = {
      0,          // vert  0, card 1, start 0
      0,1,        // vert  1, card 2, start 1
      1,2,        // vert  2, card 2, start 3
      2,          // vert  3, card 1, start 5
      0,3,        // vert  4, card 2, start 6
      0,1,3,4,    // vert  5, card 4, start 8
      1,2,4,5,    // vert  6, card 4, start 12
      2,5,        // vert  7, card 2, start 16
      3,6,        // vert  8, card 2, start 18
      3,4,6,7,    // vert  9, card 4, start 20
      4,5,7,8,    // vert  10, card 4, start 24
      5,8,        // vert  11, card 2, start 28
      6,          // vert  12, card 1, start 30
      6,7,        // vert  13, card 2, start 31
      7,8,        // vert  14, card 2, start 33
      8,          // vert  15, card 1, start 35, end 36
    };
  topoData.m_veBegins = {
      0,1,3,5,6,8,12,16,18,20,24,28,30,31,33,35,36
    };

  int numMaterials = 2;
  enum { GREEN = 0, BLUE = 1 };

  volFracs.resize(numMaterials);

  volFracs[GREEN] = {1.0, 1.0, 1.0, 1.0, 0.5, 0.2, 0.2, 0.0, 0.0};
  volFracs[BLUE] = {0.0, 0.0, 0.0, 0.0, 0.5, 0.8, 0.8, 1.0, 1.0};


  mapData.m_vertexPositions =
  {
    mir::Point2( 0.0, 3.0 ),
    mir::Point2( 1.0, 3.0 ),
    mir::Point2( 2.0, 3.0 ),
    mir::Point2( 3.0, 3.0 ),

    mir::Point2( 0.0, 2.0 ),
    mir::Point2( 1.0, 2.0 ),
    mir::Point2( 2.0, 2.0 ),
    mir::Point2( 3.0, 2.0 ),

    mir::Point2( 0.0, 1.0 ),
    mir::Point2( 1.0, 1.0 ),
    mir::Point2( 2.0, 1.0 ),
    mir::Point2( 3.0, 1.0 ),

    mir::Point2( 0.0, 0.0 ),
    mir::Point2( 1.0, 0.0 ),
    mir::Point2( 2.0, 0.0 ),
    mir::Point2( 3.0, 0.0 )
  };

  mapData.m_elementDominantMaterials = Vec<int>(numElements, NULL_MAT);
  mapData.m_elementParents = { 0,1,2,3,4,5,6,7,8 }; // For the base mesh, the parents are always themselves
  mapData.m_shapeTypes = Vec<mir::Shape>(numElements, mir::Shape::Quad);

  // Build the mesh
  mir::MIRMesh testMesh;
  testMesh.initializeMesh(verts, elems, numMaterials, topoData, mapData, volFracs);

  return testMesh;
}

//--------------------------------------------------------------------------------

mir::MIRMesh MeshTester::initTestCaseTwo()
{
  mir::CellTopologyData topoData;
  mir::CellMapData mapData;
  mir::CellData cellData;
  VolumeFractions volFracs;

  int numElements = 9;
  int numVertices = 16;
  mir::VertSet  verts(numVertices);  // Construct a vertex set with 16 vertices
  mir::ElemSet  elems(numElements);   // Construct an element set with 9 elements

  // Create the mesh connectivity information
  topoData.m_evInds = {
      0,4,5,1,     // elem 0, card 4, start 0
      1,5,6,2,     // elem 1, card 4, start 4
      2,6,7,3,     // elem 2, card 4, start 8
      4,8,9,5,     // elem 3, card 4, start 12
      5,9,10,6,    // elem 4, card 4, start 16
      6,10,11,7,   // elem 5, card 4, start 20
      8,12,13,9,   // elem 6, card 4, start 24
      9,13,14,10,  // elem 7, card 4, start 28
      10,14,15,11  // elem 8, card 4, start 32, end 36
    };

  topoData.m_evBegins = {
      0,4,8,12,16,20,24,28,32,36
    };
  topoData.m_veInds = {
      0,          // vert  0, card 1, start 0
      0,1,        // vert  1, card 2, start 1
      1,2,        // vert  2, card 2, start 3
      2,          // vert  3, card 1, start 5
      0,3,        // vert  4, card 2, start 6
      0,1,3,4,    // vert  5, card 4, start 8
      1,2,4,5,    // vert  6, card 4, start 12
      2,5,        // vert  7, card 2, start 16
      3,6,        // vert  8, card 2, start 18
      3,4,6,7,    // vert  9, card 4, start 20
      4,5,7,8,    // vert  10, card 4, start 24
      5,8,        // vert  11, card 2, start 28
      6,          // vert  12, card 1, start 30
      6,7,        // vert  13, card 2, start 31
      7,8,        // vert  14, card 2, start 33
      8,          // vert  15, card 1, start 35, end 36
    };
  topoData.m_veBegins = {
      0,1,3,5,6,8,12,16,18,20,24,28,30,31,33,35,36
    };

  int numMaterials = 3;
  enum { BLUE = 0, RED = 1, ORANGE = 2 };

  volFracs.resize(numMaterials);
  volFracs[BLUE]   = {1.0, 1.0, 1.0, 1.0, 0.5, 0.2, 0.2, 0.0, 0.0};
  volFracs[RED]    = {0.0, 0.0, 0.0, 0.0, 0.3, 0.8, 0.0, 0.3, 1.0};
  volFracs[ORANGE] = {0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.8, 0.7, 0.0};

  mapData.m_vertexPositions =
  {
    mir::Point2( 0.0, 3.0 ),
    mir::Point2( 1.0, 3.0 ),
    mir::Point2( 2.0, 3.0 ),
    mir::Point2( 3.0, 3.0 ),

    mir::Point2( 0.0, 2.0 ),
    mir::Point2( 1.0, 2.0 ),
    mir::Point2( 2.0, 2.0 ),
    mir::Point2( 3.0, 2.0 ),

    mir::Point2( 0.0, 1.0 ),
    mir::Point2( 1.0, 1.0 ),
    mir::Point2( 2.0, 1.0 ),
    mir::Point2( 3.0, 1.0 ),

    mir::Point2( 0.0, 0.0 ),
    mir::Point2( 1.0, 0.0 ),
    mir::Point2( 2.0, 0.0 ),
    mir::Point2( 3.0, 0.0 )
  };

  mapData.m_elementDominantMaterials = Vec<int>(numElements, NULL_MAT);
  mapData.m_elementParents = { 0,1,2,3,4,5,6,7,8 }; // For the base mesh, the parents are always themselves
  mapData.m_shapeTypes = Vec<mir::Shape>(numElements, mir::Shape::Quad);

  // Build the mesh
  mir::MIRMesh testMesh;
  testMesh.initializeMesh(verts, elems, numMaterials, topoData, mapData, volFracs);

  return testMesh;
}

//--------------------------------------------------------------------------------

mir::MIRMesh MeshTester::initTestCaseThree()
{
   mir::CellTopologyData topoData;
   mir::CellMapData mapData;
   mir::CellData cellData;
   VolumeFractions volFracs;

   int numElements = 4;
   int numVertices = 6;      // OR create a middle triangle with all of one material, and then a ring of triangles around it that are full of the other material

   mir::VertSet  verts = mir::VertSet(numVertices);
   mir::ElemSet  elems = mir::ElemSet(numElements);

  // Create the mesh connectivity information
  topoData.m_evInds = {
      0,1,2,     // elem 0, card 3, start 0
      1,3,4,     // elem 1, card 3, start 3
      1,4,2,     // elem 2, card 3, start 6
      2,4,5      // elem 3, card 3, start 9, end 12
    };

  topoData.m_evBegins = {
      0,3,6,9,12
    };
  topoData.m_veInds = {
      0,          // vert  0, card 1, start 0
      0,1,2,      // vert  1, card 3, start 1
      0,2,3,      // vert  2, card 3, start 4
      1,          // vert  3, card 1, start 7
      1,2,3,      // vert  4, card 3, start 8
      3           // vert  5, card 1, start 11, end 12
    };
  topoData.m_veBegins = {
      0,1,4,7,8,11,12
    };


  int numMaterials = 2;
  enum { BLUE = 0, RED = 1, };

  volFracs.resize(numMaterials);
  volFracs[BLUE] = {0.0, 0.5, 0.8, 0.5};
  volFracs[RED] =  {1.0, 0.5, 0.2, 0.5};

  mapData.m_vertexPositions =
  {
    mir::Point2( 1.0, 2.0 ),
    mir::Point2( 0.5, 1.0 ),
    mir::Point2( 1.5, 1.0 ),
    mir::Point2( 0.0, 0.0 ),
    mir::Point2( 1.0, 0.0 ),
    mir::Point2( 2.0, 0.0 )
  };

  mapData.m_elementDominantMaterials = Vec<int>(numElements, NULL_MAT);
  mapData.m_elementParents = { 0,1,2,3 }; // For the base mesh, the parents are always themselves
  mapData.m_shapeTypes = Vec<mir::Shape>(numElements, mir::Shape::Triangle);

  // Build the mesh
  mir::MIRMesh testMesh;
  testMesh.initializeMesh(verts, elems, numMaterials, topoData, mapData, volFracs);

  return testMesh;
}

//--------------------------------------------------------------------------------

mir::MIRMesh MeshTester::initTestCaseFour()
{
  mir::CellTopologyData topoData;
  mir::CellMapData mapData;
  mir::CellData cellData;
  VolumeFractions volFracs;

  int numElements = 9;
  int numVertices = 16;
  mir::VertSet  verts = mir::VertSet(numVertices);  // Construct a vertex set with 16 vertices
  mir::ElemSet  elems = mir::ElemSet(numElements);   // Construct an element set with 9 elements

  // Create the mesh connectivity information
  topoData.m_evInds = {
      0,4,5,1,     // elem 0, card 4, start 0
      1,5,6,2,     // elem 1, card 4, start 4
      2,6,7,3,     // elem 2, card 4, start 8
      4,8,9,5,     // elem 3, card 4, start 12
      5,9,10,6,    // elem 4, card 4, start 16
      6,10,11,7,   // elem 5, card 4, start 20
      8,12,13,9,   // elem 6, card 4, start 24
      9,13,14,10,  // elem 7, card 4, start 28
      10,14,15,11  // elem 8, card 4, start 32, end 36
    };

  topoData.m_evBegins = {
      0,4,8,12,16,20,24,28,32,36
    };
  topoData.m_veInds = {
      0,          // vert  0, card 1, start 0
      0,1,        // vert  1, card 2, start 1
      1,2,        // vert  2, card 2, start 3
      2,          // vert  3, card 1, start 5
      0,3,        // vert  4, card 2, start 6
      0,1,3,4,    // vert  5, card 4, start 8
      1,2,4,5,    // vert  6, card 4, start 12
      2,5,        // vert  7, card 2, start 16
      3,6,        // vert  8, card 2, start 18
      3,4,6,7,    // vert  9, card 4, start 20
      4,5,7,8,    // vert  10, card 4, start 24
      5,8,        // vert  11, card 2, start 28
      6,          // vert  12, card 1, start 30
      6,7,        // vert  13, card 2, start 31
      7,8,        // vert  14, card 2, start 33
      8,          // vert  15, card 1, start 35, end 36
    };
  topoData.m_veBegins = {
      0,1,3,5,6,8,12,16,18,20,24,28,30,31,33,35,36
    };


  mapData.m_vertexPositions =
  {
    mir::Point2( 0.0, 3.0 ),
    mir::Point2( 1.0, 3.0 ),
    mir::Point2( 2.0, 3.0 ),
    mir::Point2( 3.0, 3.0 ),

    mir::Point2( 0.0, 2.0 ),
    mir::Point2( 1.0, 2.0 ),
    mir::Point2( 2.0, 2.0 ),
    mir::Point2( 3.0, 2.0 ),

    mir::Point2( 0.0, 1.0 ),
    mir::Point2( 1.0, 1.0 ),
    mir::Point2( 2.0, 1.0 ),
    mir::Point2( 3.0, 1.0 ),

    mir::Point2( 0.0, 0.0 ),
    mir::Point2( 1.0, 0.0 ),
    mir::Point2( 2.0, 0.0 ),
    mir::Point2( 3.0, 0.0 )
  };

  int numMaterials = 2;
  enum { GREEN = 0, BLUE = 1 };

  volFracs.resize(numMaterials);

  auto& greenVolumeFractions = volFracs[GREEN];
  auto& blueVolumeFractions = volFracs[BLUE];
  const auto& points = mapData.m_vertexPositions;
  const auto& evInds = topoData.m_evInds;

  greenVolumeFractions.resize(numElements);
  blueVolumeFractions.resize(numElements);

  // Generate the element volume fractions for the circle
  mir::Point2 circleCenter(1.5, 1.5);
  axom::float64 circleRadius = 1.25;
  int gridSize = 1000;
  for (int i = 0; i < numElements; ++i)
  {
    auto vf = calculatePercentOverlapMonteCarlo(gridSize,
                                                circleCenter,
                                                circleRadius,
                                                points[evInds[i * 4 + 0]],
                                                points[evInds[i * 4 + 1]],
                                                points[evInds[i * 4 + 2]],
                                                points[evInds[i * 4 + 3]]);
    greenVolumeFractions[i] = vf;
    blueVolumeFractions[i] = 1.0 - vf;
  }

  mapData.m_elementDominantMaterials = Vec<int>(numElements, NULL_MAT);
  mapData.m_elementParents = { 0,1,2,3,4,5,6,7,8 }; // For the base mesh, the parents are always themselves
  mapData.m_shapeTypes = Vec<mir::Shape>(numElements, mir::Shape::Quad);

  // Build the mesh
  mir::MIRMesh testMesh;
  testMesh.initializeMesh(verts, elems, numMaterials, topoData, mapData, volFracs);

  return testMesh;
}

//--------------------------------------------------------------------------------

mir::MIRMesh MeshTester::createUniformGridTestCaseMesh(int gridSize, mir::Point2 circleCenter, axom::float64 circleRadius)
{
  // Generate the mesh topology
  mir::CellData cellData = generateGrid(gridSize);

  mir::VertSet  verts = mir::VertSet(cellData.m_numVerts);  // Construct the vertex set
  mir::ElemSet  elems = mir::ElemSet(cellData.m_numElems);   // Construct the element set

  int numMaterials = 2;
  enum { GREEN = 0, BLUE = 1 };

  VolumeFractions volFracs;
  volFracs.resize(numMaterials);
  volFracs[GREEN].resize(cellData.m_numElems);
  volFracs[BLUE].resize(cellData.m_numElems);

  // Generate the element volume fractions for the circle
  const int numMonteCarloSamples = 100;
  auto& pos = cellData.m_mapData.m_vertexPositions;
  const auto& evInds = cellData.m_topology.m_evInds;
  for (int i = 0; i < cellData.m_numElems; ++i)
  {
     auto vf = calculatePercentOverlapMonteCarlo(numMonteCarloSamples,
                                                 circleCenter,
                                                 circleRadius,
                                                 pos[evInds[i * 4 + 0]],
                                                 pos[evInds[i * 4 + 1]],
                                                 pos[evInds[i * 4 + 2]],
                                                 pos[evInds[i * 4 + 3]]);
     volFracs[GREEN][i] = vf;
     volFracs[BLUE][i]  = 1.0 - vf;
  }

  cellData.m_mapData.m_elementDominantMaterials = Vec<int>(cellData.m_numVerts, NULL_MAT);
  cellData.m_mapData.m_shapeTypes = Vec<mir::Shape>(cellData.m_numVerts, mir::Shape::Quad);
  cellData.m_mapData.m_elementParents.resize(cellData.m_numVerts);
  for (auto i : elems.positions() )
  {
     cellData.m_mapData.m_elementParents[i] = i;
  }

  // Build the mesh
  mir::MIRMesh testMesh;
  testMesh.initializeMesh(verts, elems, numMaterials, cellData.m_topology, cellData.m_mapData, volFracs);

  return testMesh;
}

//--------------------------------------------------------------------------------

axom::float64 MeshTester::calculatePercentOverlapMonteCarlo(int gridSize, mir::Point2 circleCenter, axom::float64 circleRadius, mir::Point2 quadP0, mir::Point2 quadP1, mir::Point2 quadP2, mir::Point2 quadP3)
{
  // Check if any of the quad's corners are within the circle
  axom::float64 distP0 = mir::utilities::distance(quadP0, circleCenter);
  axom::float64 distP1 = mir::utilities::distance(quadP1, circleCenter);
  axom::float64 distP2 = mir::utilities::distance(quadP2, circleCenter);
  axom::float64 distP3 = mir::utilities::distance(quadP3, circleCenter);

  if (distP0 < circleRadius && distP1 < circleRadius && distP2 < circleRadius && distP3 < circleRadius)
  {
    // The entire quad overlaps the circle
    return 1.0;
  }
  else if (distP0 < circleRadius || distP1 < circleRadius || distP2 < circleRadius || distP3 < circleRadius)
  {
    // Some of the quad overlaps the circle, so run the Monte Carlo sampling to determine how much
    axom::float64 delta_x = abs(quadP2.m_x - quadP1.m_x) / (double) (gridSize - 1);
    axom::float64 delta_y = abs(quadP0.m_y - quadP1.m_y) / (double) (gridSize - 1);
    int countOverlap = 0;
    for (int y = 0; y < gridSize; ++y)
    {
      for (int x = 0; x < gridSize; ++x)
      {
        mir::Point2 samplePoint(delta_x * x + quadP1.m_x, delta_y * y + quadP1.m_y);
        if (mir::utilities::distance(samplePoint, circleCenter) < circleRadius)
          ++countOverlap;
      }
    }
    return countOverlap / (double) (gridSize * gridSize);
  }
  else
  {
    // None of the quad overlaps the circle
    return 0;
  }
}

//--------------------------------------------------------------------------------

mir::CellData MeshTester::generateGrid(int gridSize)
{
  // Generate the topology for a uniform quad mesh with n x n elements automatically
  int numElements = gridSize * gridSize;
  int numVertices = (gridSize + 1) * (gridSize + 1);

   mir::CellData data;

  data.m_numVerts = numVertices;
  data.m_numElems = numElements;

  // Generate the evInds
  auto& evInds = data.m_topology.m_evInds;
  for (int eID = 0; eID < numElements; ++eID)
  {
    int row = eID / gridSize;  // note the integer division
    int vertsPerRow = gridSize + 1;
    int elemsPerRow = gridSize;

    evInds.push_back( (eID % elemsPerRow) + row * vertsPerRow + 0);
    evInds.push_back( (eID % elemsPerRow) + (row + 1) * vertsPerRow + 0);
    evInds.push_back( (eID % elemsPerRow) + (row + 1) * vertsPerRow + 1);
    evInds.push_back( (eID % elemsPerRow) + row * vertsPerRow + 1);
  }

  // Generate the evBegins
  auto& evBegins = data.m_topology.m_evBegins;
  evBegins.push_back(0);
  for (int i = 0; i < numElements; ++i)
  {
    evBegins.push_back((i + 1) * 4);
  }

  // Generate the veInds
  auto& veInds = data.m_topology.m_veInds;
  auto& veBegins = data.m_topology.m_veBegins;
  std::map<int, std::vector<int> > veInds_data;
  for (int evInd_itr = 0; evInd_itr < numElements * 4; ++evInd_itr)
  {
    int currentElementID = evInd_itr / 4; // note the integer division
    veInds_data[evInds[evInd_itr]].push_back(currentElementID);
  }
  
  for (auto itr = veInds_data.begin(); itr != veInds_data.end(); itr++)
  {
    // Sort the vector
    std::sort(itr->second.begin(), itr->second.end());

    // Add the elements associated with the current vertex to veInds
    for (unsigned long i = 0; i < itr->second.size(); ++i)
      veInds.push_back(itr->second[i]);
  }

  // Generate the veBegins
  veBegins.push_back(0);
  int currentIndexCount = 0;
  for (auto itr = veInds_data.begin(); itr != veInds_data.end(); itr++)
  {
    currentIndexCount += itr->second.size();
    veBegins.push_back(currentIndexCount);
  }

  // Generate the vertex positions
  auto& points =   data.m_mapData.m_vertexPositions;
  for (int y = gridSize; y > -1; --y)
  {
    for (int x = 0; x < gridSize + 1; ++x)
    {
      points.push_back(mir::Point2(x, y));
    }
  }

  // // Print out the results
  // printf("evInds: { ");
  // for (int i = 0; i < evInds.size(); i++)
  // {
  //   printf("%d ", evInds[i]);
  //   if ((i+1) % 4 == 0 && i != 0)
  //     printf("\n");
  // }
  // printf("}\n");

  // printf("evBegins: { ");
  // for (int i = 0; i < evBegins.size(); i++)
  // {
  //   printf("%d ", evBegins[i]);
  // }
  // printf("}\n");

  // printf("veInds: { ");
  // for (int i = 0; i < veInds.size(); i++)
  // {
  //   printf("%d ", veInds[i]);
  // }
  // printf("}\n");

  // printf("veBegins: { ");
  // for (int i = 0; i < veBegins.size(); i++)
  // {
  //   printf("%d ", veBegins[i]);
  // }
  // printf("}\n");

  // printf("points: { ");
  // for (int i = 0; i < numVertices; ++i)
  // {
  //   printf("{%.2f, %.2f} ", points[i].m_x, points[i].m_y);
  // }
  // printf("}\n");

  return data;
}

//--------------------------------------------------------------------------------

mir::MIRMesh MeshTester::initTestCaseFive(int gridSize, int numCircles)
{

  // Generate the mesh topology
  mir::CellData cellData = generateGrid(gridSize);

  mir::VertSet  verts = mir::VertSet(cellData.m_numVerts);  // Construct the vertex set
  mir::ElemSet  elems = mir::ElemSet(cellData.m_numElems);   // Construct the element set

  // Generate the element volume fractions with concentric circles
  int numMaterials = numCircles + 1;
  int defaultMaterialID = numMaterials - 1;   // default material is always the last index

  mir::Point2 circleCenter(gridSize / 2.0, gridSize / 2.0);   // all circles are centered around the same point

  // Initialize the radii of the circles
  std::vector<axom::float64> circleRadii;   
  axom::float64 maxRadius = gridSize / 2.4;   // Note: The choice of divisor is arbitrary
  axom::float64 minRadius = gridSize / 8;     // Note: The choice of divisor is arbitrary

  axom::float64 radiusDelta;
  if (numCircles <= 1)
    radiusDelta = (maxRadius - minRadius);
  else
    radiusDelta = (maxRadius - minRadius) / (double) (numCircles - 1);

  for (int i = 0; i < numCircles; ++i)
  {
    circleRadii.push_back( minRadius + (i * radiusDelta) );
  }

  // Initialize all material volume fractions to 0
  std::vector<std::vector<axom::float64> > materialVolumeFractionsData;
  for (int i = 0; i < numMaterials; ++i)
  {
    std::vector<axom::float64> tempVec;
    tempVec.resize(cellData.m_numElems);
    materialVolumeFractionsData.push_back(tempVec);
  }

  // Use the uniform sampling method to generate volume fractions for each material
  for (int eID = 0; eID < cellData.m_numElems; ++eID)
  {
    mir::Point2 v0 = cellData.m_mapData.m_vertexPositions[cellData.m_topology.m_evInds[eID * 4 + 0]]; 
    mir::Point2 v1 = cellData.m_mapData.m_vertexPositions[cellData.m_topology.m_evInds[eID * 4 + 1]]; 
    mir::Point2 v2 = cellData.m_mapData.m_vertexPositions[cellData.m_topology.m_evInds[eID * 4 + 2]]; 
    mir::Point2 v3 = cellData.m_mapData.m_vertexPositions[cellData.m_topology.m_evInds[eID * 4 + 3]];

    // Run the uniform sampling to determine how much of the current cell is composed of each material
    int materialCount[numMaterials];  for (int i = 0; i < numMaterials; ++i) materialCount[i] = 0;

    for (int matID = 0; matID < numMaterials; ++matID)
    {
      materialVolumeFractionsData[matID][eID] = materialCount[matID] / (double) (gridSize * gridSize);
    }

    axom::float64 delta_x = abs(v2.m_x - v1.m_x) / (double) (gridSize - 1);
    axom::float64 delta_y = abs(v0.m_y - v1.m_y) / (double) (gridSize - 1);

    for (int y = 0; y < gridSize; ++y)
    {
      for (int x = 0; x < gridSize; ++x)
      {
        mir::Point2 samplePoint(delta_x * x + v1.m_x, delta_y * y + v1.m_y);
        bool isPointSampled = false;
        for (int cID = 0; cID < numCircles && !isPointSampled; ++cID)
        {
          if (mir::utilities::distance(samplePoint, circleCenter) < circleRadii[cID])
          {
            materialCount[cID]++;
            isPointSampled = true;
          }
        }
        if (!isPointSampled)
        {
          // The point was not within any of the circles, so increment the count for the default material
          materialCount[defaultMaterialID]++;
        }
      }
    }

    // Assign the element volume fractions based on the count of the samples in each circle
    for (int matID = 0; matID < numMaterials; ++matID)
    {
      materialVolumeFractionsData[matID][eID] = materialCount[matID] / (double) (gridSize * gridSize);
    }
  }

  std::vector<int> elementParents; // For the base mesh, the parents are always themselves
  std::vector<int> elementDominantMaterials;
  std::vector<mir::Shape> elementShapeTypes;
  for (int i = 0; i < cellData.m_numElems; ++i)
  {
    elementParents.push_back(i);
    elementDominantMaterials.push_back(NULL_MAT);
    elementShapeTypes.push_back(mir::Shape::Quad);
  }

  CellTopologyData topology;
  topology.m_evInds = cellData.m_topology.m_evInds;
  topology.m_evBegins = cellData.m_topology.m_evBegins;
  topology.m_veInds = cellData.m_topology.m_veInds;
  topology.m_veBegins = cellData.m_topology.m_veBegins;

  CellMapData mapData;
  mapData.m_elementDominantMaterials = elementDominantMaterials;
  mapData.m_elementParents = elementParents;
  mapData.m_vertexPositions = cellData.m_mapData.m_vertexPositions;
  mapData.m_shapeTypes = elementShapeTypes;

  // Build the mesh
  mir::MIRMesh testMesh;
  testMesh.initializeMesh(verts, elems, numMaterials, topology, mapData, materialVolumeFractionsData);

  return testMesh;
}

//--------------------------------------------------------------------------------

int MeshTester::circleQuadCornersOverlaps(mir::Point2 circleCenter, axom::float64 circleRadius, mir::Point2 quadP0, mir::Point2 quadP1, mir::Point2 quadP2, mir::Point2 quadP3)
{
  // Check if any of the quad's corners are within the circle
  axom::float64 distP0 = mir::utilities::distance(quadP0, circleCenter);
  axom::float64 distP1 = mir::utilities::distance(quadP1, circleCenter);
  axom::float64 distP2 = mir::utilities::distance(quadP2, circleCenter);
  axom::float64 distP3 = mir::utilities::distance(quadP3, circleCenter);

  int numCorners = 0;

  if (distP0 < circleRadius)
    numCorners++;
  if (distP1 < circleRadius)
    numCorners++;
  if (distP2 < circleRadius)
    numCorners++;
  if (distP3 < circleRadius)
    numCorners++;

  return numCorners;
}

//--------------------------------------------------------------------------------

mir::MIRMesh MeshTester::initQuadClippingTestMesh()
{
  // Generate the mesh topology
  int gridSize = 3;
  mir::CellData cellData = generateGrid(gridSize);

  mir::VertSet  verts = mir::VertSet(cellData.m_numVerts);  // Construct the vertex set
  mir::ElemSet  elems = mir::ElemSet(cellData.m_numElems);   // Construct the element set

  int numMaterials = 2;

  std::vector<std::vector<axom::float64> > elementVF;
  elementVF.resize(numMaterials);
  elementVF[0] = {1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0};
  elementVF[1] = {0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0};

  std::vector<int> elementParents;
  std::vector<int> elementDominantMaterials;
  std::vector<mir::Shape> elementShapeTypes;
  for (int i = 0; i < cellData.m_numElems; ++i)
  {
    elementParents.push_back(i);
    elementDominantMaterials.push_back(NULL_MAT);
    elementShapeTypes.push_back(mir::Shape::Quad);
  }
  
  cellData.m_mapData.m_elementDominantMaterials = elementDominantMaterials;
  cellData.m_mapData.m_elementParents = elementParents;
  cellData.m_mapData.m_shapeTypes = elementShapeTypes;

  // Build the mesh
  mir::MIRMesh testMesh;
  testMesh.initializeMesh(verts, elems, numMaterials, cellData.m_topology, cellData.m_mapData, elementVF);

  return testMesh;
}

//--------------------------------------------------------------------------------

}
}
