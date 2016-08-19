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
/*!\file    rrlib/rtti/tGenericObjectWrapper.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief   Contains tGenericObjectWrapper
 *
 * \b tGenericObjectWrapper
 *
 * Allows wrapping an existing object as tGenericObject
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tGenericObjectWrapper_h__
#define __rrlib__rtti__tGenericObjectWrapper_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tGenericObject.h"

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
//! Wraps existing object
/*!
 * Allows wrapping an existing object as tGenericObject
 */
template <typename T>
class tGenericObjectWrapper : public tGenericObject
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tGenericObjectWrapper(T& wrapped_object) : tGenericObject(&wrapped_object, tDataType<T>())
  {
  }

  ~tGenericObjectWrapper()
  {
    data = nullptr; // ensures that data is not destructed by this object
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
