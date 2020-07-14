// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

/*!
 *******************************************************************************
 * \file DocWriter.hpp
 *
 * \brief This file contains the abstract base class definition of DocWriter.
 *******************************************************************************
 */

#ifndef INLET_DOCWRITER_HPP
#define INLET_DOCWRITER_HPP

#include <string>
#include <vector>
#include <fstream>

#include "axom/sidre.hpp"

namespace axom
{
namespace inlet
{ 

/*!
 *******************************************************************************
 * \class DocWriter
 *
 * \brief Abstract base class defining the interface of all DocWriter
 *  classes.
 *
 *  Concrete instances need to inherit from this class and implement these
 *  functions.
 *
 * \see SphinxDocWriter
 *******************************************************************************
 */
class DocWriter {
public:
  /*!
   *****************************************************************************
   * \brief Write documenation for the input deck to a given file
   *
   * This writes the documentation according to the information retrieved from 
   * the sidre group.
   *
   * \param [in]  sidreGroup The sidre group to retrieve information from
   *
   * \return true if the variable was able to be retrieved from the deck
   *****************************************************************************
   */
  virtual void writeDocuments(axom::sidre::Group* sidreGroup) = 0;

private:
  axom::sidre::Group* m_sidreRootGroup;
};

}
}

#endif
