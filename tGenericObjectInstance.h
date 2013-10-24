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
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tGenericObjectInstance_h__
#define __rrlib__rtti__tGenericObjectInstance_h__

#include "rrlib/rtti/detail/tGenericObjectBaseImpl.h"

namespace rrlib
{
namespace rtti
{
/*!
 * \author Max Reichardt
 *
 * Used for initially creating/instantiating GenericObject.
 *
 * This class should only be instantiated by tDataType !
 */
template<typename T, bool NO_ARG_CONSTRUCTOR = std::is_base_of<sDefaultStaticTypeInfo, sStaticTypeInfo<T>>::value>
class tGenericObjectInstance : public detail::tGenericObjectBaseImpl<T>
{
public:
  tGenericObjectInstance() :
    detail::tGenericObjectBaseImpl<T>(),
    wrapped_object(sStaticTypeInfo<T>::CreateByValue())
  {
    tGenericObject::wrapped = &wrapped_object;
  }

private:

  /*! Wrapped object */
  T wrapped_object;

};

template<typename T>
class tGenericObjectInstance<T, true> : public detail::tGenericObjectBaseImpl<T>
{
public:
  tGenericObjectInstance() :
    detail::tGenericObjectBaseImpl<T>(),
    wrapped_object()
  {
    tGenericObject::wrapped = &wrapped_object;
  }

private:

  /*! Wrapped object */
  T wrapped_object;

};

} // namespace
} // namespace

#endif // __rrlib__rtti__tGenericObjectInstance_h__
