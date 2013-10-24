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
/*!\file    rrlib/rtti/tGenericObject.h
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-18
 *
 * \brief   Contains tGenericObject
 *
 * \b tGenericObject
 *
 * Container/wrapper for an arbitrary object.
 *
 * Provides base functionality such as deep copying, type information
 * and serialization for wrapped object.
 * It also asserts that casting back is only possible to the original type.
 *
 * This allows to handle objects in a uniform way.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tGenericObject_h__
#define __rrlib__rtti__tGenericObject_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/serialization/serialization.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tTypedObject.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace rtti
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
class tFactory;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Generic object wrapper
/*!
 * Container/wrapper for an arbitrary object.
 *
 * Provides base functionality such as deep copying, type information
 * and serialization for wrapped object.
 * It also asserts that casting back is only possible to the original type.
 *
 * This allows to handle objects in a uniform way.
 */
class tGenericObject : public tTypedObject, private util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

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
  inline void DeepCopyFrom(const tGenericObject& source, tFactory* f = NULL)
  {
    assert((source.type == this->type) && "Types must match");
    DeepCopyFrom(source.wrapped, f);
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
  virtual bool Equals(const tGenericObject& other) = 0;

  /*!
   * \return Wrapped object (type T must match original type)
   */
  template <typename T>
  const T& GetData() const
  {
    assert(typeid(T).name() == type.GetRttiName());
    return *static_cast<const T*>(wrapped);
  }

  /*!
   * \return Wrapped object (type T must match original type)
   */
  template <typename T>
  inline T& GetData()
  {
    assert(typeid(T).name() == type.GetRttiName());
    return *static_cast<T*>(wrapped);
  }

  /*!
   * Raw void pointer to wrapped object
   */
  inline const void* GetRawDataPointer() const
  {
    return wrapped;
  }

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

  // Generic serialization
  virtual void Deserialize(serialization::tInputStream& stream) = 0;
  virtual void Deserialize(serialization::tStringInputStream& stream) = 0;
  virtual void Deserialize(const xml::tNode& node) = 0;
  virtual void Serialize(serialization::tOutputStream& stream) const = 0;
  virtual void Serialize(serialization::tStringOutputStream& stream) const = 0;
  virtual void Serialize(xml::tNode& node) const = 0;

  /*!
   * Deserialize data from binary input stream - possibly using non-binary encoding.
   *
   * \param is Binary input stream
   * \param enc Encoding to use
   */
  void Deserialize(serialization::tInputStream& stream, serialization::tDataEncoding enc);

  /*!
   * Serialize data to binary output stream - possibly using non-binary encoding.
   *
   * \param os Binary output stream
   * \param enc Encoding to use
   */
  void Serialize(serialization::tOutputStream& stream, serialization::tDataEncoding enc) const;

#endif

//----------------------------------------------------------------------
// Protected fields and constructors
//----------------------------------------------------------------------
protected:

  /*! Wrapped object */
  void* wrapped;


  /*!
   * \param wrapped Wrapped object
   * \param dt Data Type of wrapped object
   */
  tGenericObject(tType dt) :
    wrapped()
  {
    this->type = dt;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*!
   * Deep copy source object to this object
   * (types MUST match)
   *
   * \param source Source object
   */
  virtual void DeepCopyFrom(const void* source, tFactory* f) = 0;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
