//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/rtti/tTypedObject.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief   Contains tTypedObject
 *
 * \b tTypedObject
 *
 * This is the abstract base class for any object that has additional
 * type information from rrlib_rtti.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tTypedObject_h__
#define __rrlib__rtti__tTypedObject_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

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
//! Object with type information
/*!
 * This is the abstract base class for any object that has additional
 * type information from rrlib_rtti.
 */
class tTypedObject
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \return Type of object
   */
  tTypedObject() :
    type()
  {}

  virtual ~tTypedObject() {}

  /*!
   * \return Log description (default implementation is "<class name> (<pointer>)"
   */
  inline const tTypedObject& GetLogDescription() const
  {
    return *this;
  }

  /*!
   * \ŗeturn Type information for object
   */
  inline tType GetType() const
  {
    return type;
  }

//----------------------------------------------------------------------
// Protected fields and methods
//----------------------------------------------------------------------
protected:

  /*! Type information for object */
  tType type;

};

inline std::ostream& operator << (std::ostream& output, const tTypedObject* lu)
{
  output << typeid(*lu).name() << " (" << ((void*)lu) << ")";
  return output;
}

inline std::ostream& operator << (std::ostream& output, const tTypedObject& lu)
{
  output << (&lu);
  return output;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
