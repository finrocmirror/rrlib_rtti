/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2012 Max Reichardt,
 *   Robotics Research Lab, University of Kaiserslautern
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __rrlib__rtti__sStaticTypeInfo_h__
#define __rrlib__rtti__sStaticTypeInfo_h__

#include "rrlib/rtti/type_traits.h"
#include "rrlib/rtti/tIsListType.h"
#include "rrlib/rtti/detail/sStaticTypeInfoDefaultImpl.h"

namespace rrlib
{
namespace rtti
{
/*!
 * \author Max Reichardt
 *
 * This class provides various standard operations on types - as well as some info.
 * This template class should be specialized, if the defaults are not appropriate for certain types.
 *
 *  const bool stl_container_suitable;            // Is this type suitable for STL containers (std::vector<T>)?
 *  const bool shared_ptr_stl_container_suitable; // Is this type suitable for STL containers (std::vector<shared_ptr<T>>)?
 *  const bool valid_equal_to_operator;           // Has this type a valid == operator (one that does not cause compile errors)?
 *
 *  static void Clear(T)                          // Releases any shared resources an object might hold on to (currently used when recycling objects in pools)
 *  static T* Create(void* placement)             // Creates a new object of type T at specified address
 *  static T CreateByValue()                      // Creates a new object of type T and returns it by value
 *  static void DeepCopy(const T, T, tFactory*)   // Creates deep copy of object
 *
 */
template <typename T>
struct sStaticTypeInfo : public detail::sStaticTypeInfoDefaultImpl<T>
{
};

} // namespace
} // namespace

#endif // __rrlib__rtti__sStaticTypeInfo_h__
