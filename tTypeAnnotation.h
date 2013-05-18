//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/rtti/tTypeAnnotation.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-11-02
 *
 * \brief   Contains tTypeAnnotation
 *
 * \b tTypeAnnotation
 *
 * Custom annotations for types should be derived from this class
 * An instance can only be added to one type and will be deleted
 * by/with this type.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tTypeAnnotation_h__
#define __rrlib__rtti__tTypeAnnotation_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tNoncopyable.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tType.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace rtti
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Type annotation
/*!
 * Custom annotations for types should be derived from this class
 * An instance can only be added to one type and will be deleted
 * by/with this type.
 */
class tTypeAnnotation : private util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tTypeAnnotation() :
    annotated_type()
  {}

  virtual ~tTypeAnnotation() {}

  /*!
   * \return Type that is annotated
   */
  inline tType GetAnnotatedType()
  {
    return annotated_type;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tType;

  /*! Type that is annotated */
  tType annotated_type;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
