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
/*!\file    rrlib/rtti/detail/deep_copy.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__deep_copy_h__
#define __rrlib__rtti__detail__deep_copy_h__

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
#include "rrlib/serialization/serialization.h"
#endif

/*!
 * \author Max Reichardt
 *
 * This file contains diverse functions for deep-copying objects.
 *
 * If no function matches, the default serialization-based mechanism
 * is used as a fallback.
 */

namespace rrlib
{
namespace rtti
{
namespace detail
{

struct tFactoryWrapper
{
  rtti::tFactory* factory;
  tFactoryWrapper(rtti::tFactory* f) : factory(f) {}
};

// if copyFrom is defined, take this
template <typename T>
inline void DeepCopy(const T& t, T& t2, rtti::tFactory* f, decltype(((T*)NULL)->CopyFrom(*((T*)NULL)))* = NULL)
{
  t2.CopyFrom(t);
}

template <typename T, bool B>
struct tCopyImpl
{
  inline static void DeepCopyImpl(const T& t, T& t2, rtti::tFactory* f)
  {
    t2 = t;
  }
};

template <typename T>
struct tCopyImpl<T, true>
{
  inline static void DeepCopyImpl(const T& src, T& dest, rtti::tFactory* f)
  {
#if (! __clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8))
    if (std::has_trivial_destructor<T>::value)
#else
    if (std::is_trivially_destructible<T>::value)
#endif
    {
      memcpy(&dest, &src, sizeof(T)); // (heuristic, however, I have never encountered a type where this is invalid)
    }
    else
    {
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
      rrlib::serialization::SerializationBasedDeepCopy(src, dest, f);
#else
      RRLIB_LOG_PRINT(ERROR, "Cannot deep copy object of type ", Demangle(typeid(T).name()), ". This will likely cause malfunction.");
#endif
    }
  }
};

// if not, use = operator
template <typename T>
inline void DeepCopy(const T& t, T& t2, tFactoryWrapper f)
{
  tCopyImpl<T, std::is_base_of<util::tNoncopyable, T>::value>::DeepCopyImpl(t, t2, f.factory);
}

template <typename T>
void DeepCopyImpl(const T& t, T& t2, rtti::tFactory* f)
{
  DeepCopy(t, t2, f);
}

// for std::vector<bool> support
inline void DeepCopyImpl(const bool& b, std::vector<bool>::reference b2, rtti::tFactory* f)
{
  b2 = b;
}

} // namespace
} // namespace
} // namespace

#endif // __rrlib__rtti__detail__deep_copy_h__
