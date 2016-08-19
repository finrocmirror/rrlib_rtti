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
/*!\file    rrlib/rtti/detail/type_traits.h
 *
 * \author  Max Reichardt
 *
 * \date    2015-05-22
 *
 * Contains type trait implementation helper classes and structs
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__type_traits_h__
#define __rrlib__rtti__detail__type_traits_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/util/tManagedConstCharPointer.h"

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
// Function declarations
//----------------------------------------------------------------------

/*!
 * Provides normalized integer types for specified size and signedness
 */
template <size_t SIZE, bool UNSIGNED>
struct NormalizedIntegerType
{
  typedef void type;
};

template<>
struct NormalizedIntegerType<1, false>
{
  typedef int8_t type;
};

template<>
struct NormalizedIntegerType<1, true>
{
  typedef uint8_t type;
};

template<>
struct NormalizedIntegerType<2, false>
{
  typedef int16_t type;
};

template<>
struct NormalizedIntegerType<2, true>
{
  typedef uint16_t type;
};

template<>
struct NormalizedIntegerType<4, false>
{
  typedef int type;
};

template<>
struct NormalizedIntegerType<4, true>
{
  typedef unsigned int type;
};

template<>
struct NormalizedIntegerType<8, false>
{
  typedef long long type;
};

template<>
struct NormalizedIntegerType<8, true>
{
  typedef unsigned long long type;
};

/*!
 * Type trait to determine whether T has a CopyFrom member function
 */
template <typename T>
struct HasCopyFromMethod
{
  template <typename U>
  static U &Make();

  template <typename U = T>
  static int16_t Test(decltype(Make<U>().CopyFrom(Make<const U>()))*);

  static int32_t Test(...);

  enum { value = sizeof(Test(static_cast<void*>(nullptr))) == sizeof(int16_t) }; // g++ 4.8 requires this cast somehow
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
