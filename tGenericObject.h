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

#ifndef __rrlib__rtti__tGenericObject_h__
#define __rrlib__rtti__tGenericObject_h__

#include "rrlib/rtti/tDataTypeBase.h"
#include "rrlib/rtti/tTypedObject.h"
#include <assert.h>
#include <typeinfo>
#include <cstring>


namespace rrlib
{
namespace rtti
{
class tFactory;
class tGenericObjectManager;

/*!
 * \author Max Reichardt
 *
 * Container/wrapper for an arbitrary object.
 *
 * Provides base functionality such as deep copying, type information
 * and serialization.
 * It also assert that casting back is only possible to the original type.
 *
 * This allows to handle objects in a uniform way.
 *
 * Memory Layout of all subclasses: vtable ptr | datatype ptr | object ptr | management info raw memory of size M
 */
class tGenericObject : public tTypedObject, boost::noncopyable
{
protected:

  /*! Wrapped object */
  void* wrapped;

#if __SIZEOF_POINTER__ == 4
  int fill_dummy; // fill 4 byte to ensure that managers are 8-byte-aligned on 32 bit platforms
#endif

  /*!
   * Deep copy source object to this object
   * (types MUST match)
   *
   * \param source Source object
   */
  virtual void DeepCopyFrom(const void* source, tFactory* f = NULL) = 0;

public:

  static const size_t cMANAGER_OFFSET = (sizeof(void*) == 4) ? 16 : 24;

  /*!
   * \param wrapped Wrapped object
   * \param dt Data Type of wrapped object
   */
  tGenericObject(tDataTypeBase dt) :
    wrapped()
  {
    this->type = dt;
  }

  // to ensure that all generic objects have virtual destructor
  virtual ~tGenericObject() {}

  /*!
   * Clear any shared resources that this object holds on to
   * (e.g. for reusing object in pool)
   */
  virtual void Clear() = 0;

  /*!
   * Deep copy source object to this object
   * (types MUST match)
   *
   * \param source Source object
   */
  inline void DeepCopyFrom(const tGenericObject* source, tFactory* f = NULL)
  {
    assert(((source->type == this->type)) && "Types must match");

    DeepCopyFrom(source->wrapped, f);
  }

  /*!
   * Does object equal other object?
   * If rrlib_serialization is not present, this only works for types
   * that have the == operator implemented (or the two objects share the same address)
   *
   * (not very efficient/RT-capable - should therefore not be called regular loops)
   *
   * \return True, if...
   *  1) both objects have the same address
   *  2) the == operator returns true for both objects
   *  3) T has trivial destructor and memcmp returns 0 (heuristic, however, I have never encountered a type where this is invalid)
   *  4) rrlib_serialization is available and both objects are serialized to the same binary data (usually they are equal then)
   */
  virtual bool Equals(tGenericObject& other)
  {
    if (wrapped == other.wrapped)
    {
      return true;
    }
    else if (GetType() == other.GetType())
    {
      bool cmp = (type.GetTypeTraits() & trait_flags::cHAS_TRIVIAL_DESTRUCTOR) && memcmp(wrapped, other.wrapped, GetType().GetSize()) == 0;
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
      return cmp || serialization::SerializationEquals(*this, other);
#else
      return cmp;
#endif
    }
    return false;
  }

  /*!
   * \return Wrapped object (type T must match original type)
   */
  template <typename T>
  const T* GetData() const
  {
    assert(typeid(T).name() == type.GetRttiName());
    return static_cast<const T*>(wrapped);
  }

  /*!
   * \return Wrapped object (type T must match original type)
   */
  template <typename T>
  inline T* GetData()
  {
    assert(typeid(T).name() == type.GetRttiName());
    return static_cast<T*>(wrapped);
  }

  /*!
   * \return Management information for this generic object.
   */
  inline tGenericObjectManager* GetManager()
  {
    return reinterpret_cast<tGenericObjectManager*>(reinterpret_cast<char*>(this) + cMANAGER_OFFSET);
  }

  /*!
   * Raw void pointer to wrapped object
   */
  inline const void* GetRawDataPtr() const
  {
    return wrapped;
  }

};

} // namespace
} // namespace

#endif // __rrlib__rtti__tGenericObject_h__
