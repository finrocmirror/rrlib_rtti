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
 * \brief   Contains tGenericObjectBaseImpl
 *
 * \b tGenericObjectBaseImpl
 *
 * This class implements all type-specific generic operations of
 * tGenericObject.
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__tGenericObjectBaseImpl_h__
#define __rrlib__rtti__detail__tGenericObjectBaseImpl_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/serialization/tStringInputStream.h"
#include "rrlib/serialization/tStringOutputStream.h"
#include "rrlib/serialization/type_traits.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tGenericObject.h"

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
//! Generic object implementation
/*!
 * This class implements all type-specific generic operations of
 * tGenericObject.
 */
template<typename T>
class tGenericObjectBaseImpl : public tGenericObject
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  virtual void Clear() override
  {
    //TODO
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

  virtual bool Equals(const tGenericObject& other) override
  {
    return wrapped == other.GetRawDataPointer() || (GetType() == other.GetType() && GenericOperations<T>::Equals(this->GetData<T>(), other.GetData<T>()));
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

//----------------------------------------------------------------------
// Protected constructor and methods
//----------------------------------------------------------------------
protected:

  tGenericObjectBaseImpl() :
    tGenericObject(tDataType<T>())
  {}

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  virtual void DeepCopyFrom(const void* source, tFactory* f) override
  {
    DeepCopyFromImpl(*static_cast<const T*>(source), f);
  }

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
