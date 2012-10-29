/**
 * You received this file as part of RRLib serialization
 *
 * Copyright (C) 2011 Max Reichardt,
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

#ifndef __rrlib__rtti__detail__tListInfo_h__
#define __rrlib__rtti__detail__tListInfo_h__

#include "rrlib/rtti/tDataTypeBase.h"
#include <memory>

namespace rrlib
{
namespace rtti
{
namespace detail
{
/*!
 * \author Max Reichardt
 *
 * List-related information for data type T
 *
 * type:               Data type type
 * tListType:          List type (if plain type)
 * tSharedPtrListType: Shared Pointer List type (if plain type)
 * tElementType:       Element type (if list type)
 */
class tNothing {};

template <typename T>
struct tListInfo
{
  static const tDataTypeBase::tType type = tDataTypeBase::tType::PLAIN;
  typedef std::vector<T> tListType;
  typedef std::vector<std::shared_ptr<T> > tSharedPtrListType;
  typedef tNothing tElementType;
  static std::string GetName()
  {
    return tDataTypeBase::GetDataTypeNameFromRtti(typeid(T).name());
  }
};

template <typename T>
struct tListInfoBase
{
  static const tDataTypeBase::tType type = tDataTypeBase::tType::LIST;
  typedef tNothing tListType;
  typedef tNothing tSharedPtrListType;
  typedef T tElementType;
  static std::string GetName()
  {
    return std::string("List<") + tListInfo<T>::GetName() + ">";
  }
};

template <typename T>
struct tListInfoBase<std::shared_ptr<T> >
{
  static const tDataTypeBase::tType type = tDataTypeBase::tType::PTR_LIST;
  typedef tNothing tListType;
  typedef tNothing tSharedPtrListType;
  typedef T tElementType;
  static std::string GetName()
  {
    return std::string("List<") + tListInfo<T>::GetName() + "*>";
  }
};

template <typename T>
struct tListInfo<std::vector<T> > : public tListInfoBase<T> {};

} // namespace
} // namespace
} // namespace

#endif // __rrlib__rtti__detail__tListInfo_h__
