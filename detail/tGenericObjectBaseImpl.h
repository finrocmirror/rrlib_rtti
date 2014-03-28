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

#include "rrlib/serialization/tStringInputStream.h"
#include "rrlib/serialization/tStringOutputStream.h"
#include "rrlib/serialization/type_traits.h"

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

template<typename T>
class tGenericObjectBaseImpl : public tGenericObject
{
protected:

  tGenericObjectBaseImpl() :
    tGenericObject(tDataType<T>())
  {}

  virtual void DeepCopyFrom(const void* source, tFactory* f) override
  {
    DeepCopyFromImpl(*static_cast<const T*>(source), f);
  }

public:

  virtual void Clear() override
  {
    //TODO
  }

  virtual bool Equals(const tGenericObject& other) override
  {
    return wrapped == other.GetRawDataPointer() || (GetType() == other.GetType() && GenericOperations<T>::Equals(this->GetData<T>(), other.GetData<T>()));
  }

  /*!
   * Deep copy source object to this object
   *
   * \param source Source object
   */
  inline void DeepCopyFromImpl(const T& source, tFactory* f = NULL)
  {
    GenericOperations<T>::DeepCopy(source, this->GetData<T>());
  }

  virtual void Deserialize(serialization::tInputStream& is) override
  {
    serialization::Deserialize(is, tGenericObject::GetData<T>());
  }

  virtual void Deserialize(serialization::tStringInputStream& is) override
  {
    serialization::Deserialize(is, tGenericObject::GetData<T>());
  }

  virtual void Deserialize(const xml::tNode& node) override
  {
    serialization::Deserialize(node, tGenericObject::GetData<T>());
  }

  virtual void Serialize(serialization::tOutputStream& os) const override
  {
    serialization::Serialize(os, tGenericObject::GetData<T>());
  }

  virtual void Serialize(serialization::tStringOutputStream& os) const override
  {
    serialization::Serialize(os, tGenericObject::GetData<T>());
  }

  virtual void Serialize(xml::tNode& node) const override
  {
    serialization::Serialize(node, tGenericObject::GetData<T>());
  }
};

} // namespace
} // namespace
} // namespace

#endif // __rrlib__rtti__tGenericObjectBaseImpl_h__
