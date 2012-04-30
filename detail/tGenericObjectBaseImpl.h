/**
 * You received this file as part of RRLib serialization
 *
 * Copyright (C) 2011-2012 Max Reichardt,
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

#ifndef __rrlib__rtti__tGenericObjectBaseImpl_h__
#define __rrlib__rtti__tGenericObjectBaseImpl_h__

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
#include "rrlib/serialization/tStringInputStream.h"
#include "rrlib/serialization/tStringOutputStream.h"
#include "rrlib/serialization/type_traits.h"
#endif

#include "rrlib/rtti/tGenericObject.h"

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace rrlib
{
namespace rtti
{
class tFactory;

namespace detail
{

template <typename T, bool EQ_OP>
class tGenericObjectBaseImplEq : public tGenericObject
{
protected:
  using tGenericObject::wrapped;

  tGenericObjectBaseImplEq() : tGenericObject(tDataType<T>()) {}
};

template <typename T>
class tGenericObjectBaseImplEq<T, true> : public tGenericObject
{
protected:

  using tGenericObject::wrapped;

  tGenericObjectBaseImplEq() : tGenericObject(tDataType<T>()) {}

  virtual bool Equals(tGenericObject& other)
  {
    return wrapped == other.GetRawDataPtr() || (GetType() == other.GetType() && (*GetData<T>() == *other.GetData<T>()));
  }
};

template<typename T>
class tGenericObjectBaseImpl : public tGenericObjectBaseImplEq<T, sStaticTypeInfo<T>::valid_equal_to_operator>
{
protected:

  using tGenericObject::wrapped;

  tGenericObjectBaseImpl() :
    tGenericObjectBaseImplEq<T, sStaticTypeInfo<T>::valid_equal_to_operator>()
  {}

  virtual void DeepCopyFrom(const void* source, tFactory* f)
  {
    DeepCopyFromImpl(*static_cast<const T*>(source), f);
  }

public:

  virtual void Clear()
  {
    sStaticTypeInfo<T>::Clear(*tGenericObject::GetData<T>());
  }

  /*!
   * Deep copy source object to this object
   *
   * \param source Source object
   */
  inline void DeepCopyFromImpl(const T& source, tFactory* f = NULL)
  {
    sStaticTypeInfo<T>::DeepCopy(source, *tGenericObject::GetData<T>(), f);
  }

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

  virtual void Deserialize(serialization::tInputStream& is)
  {
    serialization::Deserialize(is, *tGenericObject::GetData<T>());
  }

  virtual void Deserialize(serialization::tStringInputStream& is)
  {
    serialization::Deserialize(is, *tGenericObject::GetData<T>());
  }

  virtual void Deserialize(const xml2::tXMLNode& node)
  {
    serialization::Deserialize(node, *tGenericObject::GetData<T>());
  }

  virtual void Serialize(serialization::tOutputStream& os) const
  {
    serialization::Serialize(os, *tGenericObject::GetData<T>());
  }

  virtual void Serialize(serialization::tStringOutputStream& os) const
  {
    serialization::Serialize(os, *tGenericObject::GetData<T>());
  }

  virtual void Serialize(xml2::tXMLNode& node) const
  {
    serialization::Serialize(node, *tGenericObject::GetData<T>());
  }

#endif
};

} // namespace
} // namespace
} // namespace

#endif // __rrlib__rtti__tGenericObjectBaseImpl_h__
