//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) AG Robotersysteme TU Kaiserslautern
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
/*!\file    rrlib/rtti/detail/tGenericObjectInstanceEmplaced.h
 *
 * \author  Max Reichardt
 *
 * \date    2014-06-23
 *
 * \brief   Contains tGenericObjectInstanceEmplaced
 *
 * \b tGenericObjectInstanceEmplaced
 *
 * A generic object that receives memory to initialize an object T in - and wrap it
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__tGenericObjectInstanceEmplaced_h__
#define __rrlib__rtti__detail__tGenericObjectInstanceEmplaced_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace rtti
{
namespace detail
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Generic object using specified memory address
/*!
 * A generic object that receives memory to initialize an object T in - and wrap it.
 * Memory block needs to have size sizeof(T)
 */
template<typename T, bool NO_ARG_CONSTRUCTOR = std::is_base_of<serialization::DefaultImplementation, serialization::DefaultInstantiation<T>>::value>
class tGenericObjectInstanceEmplaced : public detail::tGenericObjectBaseImpl<T>
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tGenericObjectInstanceEmplaced(void* address) :
    detail::tGenericObjectBaseImpl<T>()
  {
    new(address) T(serialization::DefaultInstantiation<T>::Create());
    tGenericObject::wrapped = address;
  }

};

// Specialization for when default constructor is available
template<typename T>
class tGenericObjectInstanceEmplaced<T, true> : public detail::tGenericObjectBaseImpl<T>
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tGenericObjectInstanceEmplaced(void* address) :
    detail::tGenericObjectBaseImpl<T>()
  {
    new(address) T();
    tGenericObject::wrapped = address;
  }

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
