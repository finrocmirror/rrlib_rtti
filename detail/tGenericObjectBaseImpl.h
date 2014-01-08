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
/*!\file    rrlib/rtti/detail/tGenericObjectBaseImpl.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
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
namespace xml
{
class tNode;
}
}

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

  virtual bool Equals(const tGenericObject& other)
  {
    if (this->GetRawDataPointer() == other.GetRawDataPointer())
    {
      return true;
    }
#if (! __clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8))
    if (std::has_trivial_destructor<T>::value)
#else
    if (std::is_trivially_destructible<T>::value)
#endif
    {
      return GetType() == other.GetType() && memcmp(GetRawDataPointer(), other.GetRawDataPointer(), GetType().GetSize()) == 0;
    }
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
    return GetType() == other.GetType() && serialization::SerializationEquals(GetData<T>(), other.GetData<T>());
#else
    return false;
#endif
  }
};

template <typename T>
class tGenericObjectBaseImplEq<T, true> : public tGenericObject
{
protected:

  using tGenericObject::wrapped;

  tGenericObjectBaseImplEq() : tGenericObject(tDataType<T>()) {}

  virtual bool Equals(const tGenericObject& other)
  {
    return wrapped == other.GetRawDataPointer() || (GetType() == other.GetType() && (GetData<T>() == other.GetData<T>()));
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
    sStaticTypeInfo<T>::Clear(tGenericObject::GetData<T>());
  }

  /*!
   * Deep copy source object to this object
   *
   * \param source Source object
   */
  inline void DeepCopyFromImpl(const T& source, tFactory* f = NULL)
  {
    sStaticTypeInfo<T>::DeepCopy(source, tGenericObject::GetData<T>(), f);
  }

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

  virtual void Deserialize(serialization::tInputStream& is)
  {
    serialization::Deserialize(is, tGenericObject::GetData<T>());
  }

  virtual void Deserialize(serialization::tStringInputStream& is)
  {
    serialization::Deserialize(is, tGenericObject::GetData<T>());
  }

  virtual void Deserialize(const xml::tNode& node)
  {
    serialization::Deserialize(node, tGenericObject::GetData<T>());
  }

  virtual void Serialize(serialization::tOutputStream& os) const
  {
    serialization::Serialize(os, tGenericObject::GetData<T>());
  }

  virtual void Serialize(serialization::tStringOutputStream& os) const
  {
    serialization::Serialize(os, tGenericObject::GetData<T>());
  }

  virtual void Serialize(xml::tNode& node) const
  {
    serialization::Serialize(node, tGenericObject::GetData<T>());
  }

#endif
};

} // namespace
} // namespace
} // namespace

#endif // __rrlib__rtti__tGenericObjectBaseImpl_h__
