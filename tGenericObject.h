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

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tTypedPointer.h"

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
 * It also asserts that casting back is only possible to the original/underlying type.
 *
 * This allows to handle objects in a uniform way.
 *
 * Unlike tTypedPointer, tGenericObject is also concerned with
 * creation, destruction, and ownership of the object it points to.
 * The wrapped pointer is never nullptr.
 *
 * Generic objects are constructed with the tType class.
 */
class tGenericObject : private util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  ~tGenericObject()
  {
    type.DestructInstance(data);
    data = nullptr;
  }

  // only allow casting to const base classes - everything else would be dangerous
  operator const tTypedConstPointer&() const
  {
    return reinterpret_cast<const tTypedConstPointer&>(*this);
  }
  operator const tTypedPointer&()
  {
    return reinterpret_cast<const tTypedPointer&>(*this);
  }

  /*!
   * Deep copy data to data pointed to by this pointer.
   * A deep copy means that the destination object must not change if the source object is modified or deleted.
   * Serialization of source and destination objects are equal after calling this.
   * Caller must ensure that source and destination have the same (underlying) type and are not nullptr.
   *
   * \param source Pointer to data to be copied
   * \throws May throw std::exception on invalid arguments or other failure
   */
  inline void DeepCopyFrom(const tTypedConstPointer& source)  // only non-const for tGenericObject
  {
    reinterpret_cast<const tTypedPointer&>(*this).DeepCopyFrom(source);
  }

  /*!
   * Deserialize data from input stream
   *
   * \param stream Input stream
   * \throws Throws std::exception on invalid data in stream
   */
  inline void Deserialize(serialization::tInputStream& stream)  // only non-const for tGenericObject
  {
    reinterpret_cast<const tTypedPointer&>(*this).Deserialize(stream);
  }

  /*!
   * Deserialize data from input stream
   *
   * \param stream Input stream
   * \throws Throws std::exception on invalid data in stream
   */
  inline void Deserialize(serialization::tStringInputStream& stream)  // only non-const for tGenericObject
  {
    reinterpret_cast<const tTypedPointer&>(*this).Deserialize(stream);
  }

  /*!
   * Deserialize data from XML node
   *
   * \param node XML node
   * \throws Throws std::exception on invalid data in XML node
   */
  void Deserialize(const xml::tNode& node)  // only non-const for tGenericObject
  {
    reinterpret_cast<const tTypedPointer&>(*this).Deserialize(node);
  }

  /*!
   * Deserialize data from binary input stream - possibly using non-binary encoding.
   *
   * \param stream Binary input stream
   * \param encoding Encoding to use
   */
  void Deserialize(serialization::tInputStream& stream, serialization::tDataEncoding encoding)  // only non-const for tGenericObject
  {
    reinterpret_cast<const tTypedPointer&>(*this).Deserialize(stream, encoding);
  }

  /*!
   * Returns whether data of two pointers is equal.
   * Serializing equal objects must produce identical data.
   *
   * \param other Data to compare with
   */
  bool Equals(const tTypedConstPointer& other) const
  {
    return reinterpret_cast<const tTypedConstPointer&>(*this).Equals(other);
  }

  /*!
   * Same as Equals, but also returns true if only underlying types are equal.
   *
   * \param other Data to compare with
   */
  bool EqualsUnderlying(const tTypedConstPointer& other) const
  {
    return reinterpret_cast<const tTypedConstPointer&>(*this).EqualsUnderlying(other);
  }

  /*!
   * \return Wrapped object (type T must match original type)
   */
  template <typename T>
  inline const T& GetData() const
  {
    assert(typeid(typename NormalizedType<T>::type).name() == type.GetRttiName());
    return *static_cast<const T*>(data);
  }

  /*!
   * \return Wrapped object (type T must match original type)
   */
  template <typename T>
  inline T& GetData()
  {
    assert(typeid(typename NormalizedType<T>::type).name() == type.GetRttiName());
    return *static_cast<T*>(data);
  }

  /*!
   * \return Raw void pointer to wrapped object
   */
  inline void* GetRawDataPointer()
  {
    return data;
  }
  inline const void* GetRawDataPointer() const
  {
    return data;
  }

  /*!
   * \ŗeturn Type information for object
   */
  inline const tType& GetType() const
  {
    return type;
  }

  /*!
   * Obtain element from vector.
   *
   * \param data Data pointer
   * \param size Index of element
   * \return
   * - If this points to std::vector, returns &std::vector[index]
   * - Otherwise, returns this object if index == 0
   * \throws Throws std::invalid_argument if index is >= of what is returned by tGetVectorSize
   */
  inline tTypedPointer GetVectorElement(size_t index)
  {
    return reinterpret_cast<const tTypedPointer&>(*this).GetVectorElement(index);
  }
  inline tTypedConstPointer GetVectorElement(size_t index) const
  {
    return reinterpret_cast<const tTypedPointer&>(*this).GetVectorElement(index);
  }

  /*!
   * \param data Data pointer
   * \return
   * - If this points to std::vector, returns std::vector::size()
   * - If nullptr, returns 0
   * - Otherwise, returns 1
   */
  inline size_t GetVectorSize() const
  {
    return reinterpret_cast<const tTypedConstPointer&>(*this).GetVectorSize();
  }

  /*!
   * Resizes std::vector.
   *
   * \param data Pointer to std::vector
   * \param new_size New size
   * \throws Throws std::invalid_argument 'data' does not point std::vector
   */
  void ResizeVector(size_t new_size)
  {
    reinterpret_cast<const tTypedPointer&>(*this).ResizeVector(new_size);
  }

  /*!
   * Serialize data
   *
   * \param target Target to serialize to (output stream or XML node)
   */
  template <typename TTarget>
  inline void Serialize(TTarget& target) const
  {
    reinterpret_cast<const tTypedConstPointer&>(*this).Serialize(target);
  }

  /*!
   * Serialize data to binary output stream - possibly using non-binary encoding.
   *
   * \param stream Binary output stream
   * \param encoding Encoding to use
   */
  void Serialize(serialization::tOutputStream& stream, serialization::tDataEncoding encoding) const
  {
    reinterpret_cast<const tTypedConstPointer&>(*this).Serialize(stream, encoding);
  }

  /*!
   * \return String representation of object this points to. This is the serialized value for string serializable types. Otherwise it is type name + pointer.
   */
  std::string ToString() const
  {
    return reinterpret_cast<const tTypedConstPointer&>(*this).ToString();
  }

//----------------------------------------------------------------------
// Protected fields and constructors
//----------------------------------------------------------------------
protected:

  friend class tType;

  tGenericObject(void* pointer, const tType& type) : data(pointer), type(type)
  {
    assert((pointer == nullptr || (type.GetTypeTraits() & trait_flags::cTYPE_CLASSIFICATION_BITS) != static_cast<uint>(tTypeClassification::RPC_TYPE)) && "Only data types are valid");
  }

  /*! Raw pointer to data/object */
  void* data;

  /*! Runtime type information for the data/object that 'data' points to */
  tType type;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}

#include "rrlib/rtti/tType.hpp"

#endif
