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
/*!\file    rrlib/rtti/tGenericObjectInstance.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief   Contains tGenericObjectInstance
 *
 * \b tGenericObjectInstance
 *
 * A generic object that allocates and owns its wrapped data
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tGenericObjectInstance_h__
#define __rrlib__rtti__tGenericObjectInstance_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/detail/tGenericObjectBaseImpl.h"

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
//! Generic object owning buffer
/*!
 * A generic object that allocates and owns its wrapped data
 */
template<typename T, bool NO_ARG_CONSTRUCTOR = std::is_base_of<serialization::DefaultImplementation, serialization::DefaultInstantiation<T>>::value>
class tGenericObjectInstance : public detail::tGenericObjectBaseImpl<typename NormalizedType<T>::type>
{
  typedef typename NormalizedType<T>::type tBuffer;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tGenericObjectInstance() :
    detail::tGenericObjectBaseImpl<tBuffer>(),
    wrapped_object(serialization::DefaultInstantiation<tBuffer>::Create())
  {
    tGenericObject::wrapped = &wrapped_object;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Wrapped object */
  tBuffer wrapped_object;
};


// Specialization for when default constructor is available
template<typename T>
class tGenericObjectInstance<T, true> : public detail::tGenericObjectBaseImpl<typename NormalizedType<T>::type>
{
  typedef typename NormalizedType<T>::type tBuffer;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tGenericObjectInstance() :
    detail::tGenericObjectBaseImpl<tBuffer>(),
    wrapped_object()
  {
    tGenericObject::wrapped = &wrapped_object;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Wrapped object */
  tBuffer wrapped_object;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
