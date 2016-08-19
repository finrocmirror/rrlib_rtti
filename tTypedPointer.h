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
/*!\file    rrlib/rtti/tTypedPointer.h
 *
 * \author  Max Reichardt
 *
 * \date    2016-07-21
 *
 * \brief   Contains tTypedPointer
 *
 * \b tTypedPointer
 *
 * Pointer with runtime type information attached.
 * It points to an object whose type is not known at compile time.
 * The rrlib_rtti runtime information allows to perform various generic operations
 * with the object (including type conversion with rrlib_rtti_conversion) -
 * even without casting it back to the original type.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tTypedPointer_h__
#define __rrlib__rtti__tTypedPointer_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tType.h"

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

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Pointer with runtime type information attached.
/*!
 * Pointer with runtime type information attached.
 * It points to data whose type is not known at compile time.
 * The rrlib_rtti runtime information allows to perform various generic operations
 * with the data (including type conversion with rrlib_rtti_conversion) -
 * even without casting it back to the original type.
 *
 * There is tTypedPointer for non-const data and tTypedConstPointer for const data
 */
class tTypedConstPointer
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tTypedConstPointer() : data(nullptr), type() {}
  tTypedConstPointer(const void* pointer, const tType& type) : data(pointer), type(type)
  {
    assert((pointer == nullptr || (type.GetTypeTraits() & trait_flags::cIS_DATA_TYPE)) && "Only data types are valid");
  }
  template <typename T>
  tTypedConstPointer(const T* object) : data(object), type(tDataType<T>())
  {}

  inline operator bool() const
  {
    return data != nullptr;
  }

  /*!
   * Returns whether data of two pointers is equal.
   * Serializing equal objects must produce identical data.
   *
   * \param other Data to compare with
   */
  bool Equals(const tTypedConstPointer& other) const
  {
    if (other.GetRawDataPointer() == data)
    {
      return true;
    }
    if (type == this->GetType() && data && other.GetRawDataPointer())
    {
      return (type.GetTypeTraits() & trait_flags::cSUPPORTS_BITWISE_COPY) ? memcmp(data, other.GetRawDataPointer(), type.GetSize()) == 0 : (*type.GetBinaryOperations().equals)(*this, other);
    }
    return false;
  }

  /*!
   * Same as Equals, but also returns true if only underlying types are equal.
   *
   * \param other Data to compare with
   */
  bool EqualsUnderlying(const tTypedConstPointer& other) const
  {
    if (other.GetRawDataPointer() == data)
    {
      return true;
    }
    if (data && other.GetRawDataPointer() && type.GetUnderlyingType() == this->GetType().GetUnderlyingType())
    {
      return (type.GetTypeTraits() & trait_flags::cSUPPORTS_BITWISE_COPY) ? memcmp(data, other.GetRawDataPointer(), type.GetSize()) == 0 : (*type.GetBinaryOperations().equals)(*this, other);
    }
    return false;
  }

  /*!
   * \return Wrapped object (type T must match original type)
   */
  template <typename T>
  inline const T* Get() const
  {
    assert(typeid(typename NormalizedType<T>::type).name() == type.GetRttiName());
    return static_cast<const T*>(data);
  }

  /*!
   * Raw void pointer to wrapped object
   */
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
   * \return Wrapped object (no type checks via assertions are performed)
   */
  template <typename T>
  inline const T* GetUnchecked() const
  {
    return static_cast<const T*>(data);
  }

  /*!
   * Obtain element from vector.
   *
   * \param data Data pointer
   * \param size Index of element
   * \return
   * - If index >= return value of tGetVectorSize(), returns nullptr
   * - If data points to std::vector, returns &std::vector[index]
   * - Otherwise, returns contents of data
   */
  inline tTypedConstPointer GetVectorElement(size_t index) const
  {
    if (type.IsListType())
    {
      return (*type.GetBinaryOperationsVector().get_vector_element)(*this, index);
    }
    else if (index >= 1)
    {
      return tTypedConstPointer();
    }
    return *this;
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
    return type.IsListType() ? (*type.GetBinaryOperationsVector().get_vector_size)(*this) : (data ? 1 : 0);
  }

  /*!
   * Serialize data to output stream
   *
   * \param stream Output stream
   */
  inline void Serialize(serialization::tOutputStream& stream) const
  {
    if (data && (type.GetTypeTraits() & trait_flags::cIS_BINARY_SERIALIZABLE))
    {
      (*type.GetBinarySerialization().serialize)(stream, *this);
    }
    else
    {
      throw std::runtime_error("Serialization not supported");
    }
  }

  /*!
   * Serialize data to output stream
   *
   * \param stream Output stream
   */
  void Serialize(serialization::tStringOutputStream& stream) const;

  /*!
   * Serialize data to XML node
   *
   * \param node XML node
   */
  void Serialize(xml::tNode& node) const;

  /*!
   * Serialize data to binary output stream - possibly using non-binary encoding.
   *
   * \param stream Binary output stream
   * \param encoding Encoding to use
   */
  void Serialize(serialization::tOutputStream& stream, serialization::tDataEncoding encoding) const;

//----------------------------------------------------------------------
// Private methods and fields
//----------------------------------------------------------------------
private:

  /*! Raw pointer to data/object */
  const void* data;

  /*! Runtime type information for the data/object that 'data' points to */
  tType type;
};


class tTypedPointer
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tTypedPointer() : data(nullptr), type() {}
  tTypedPointer(void* pointer, const tType& type) : data(pointer), type(type)
  {
    assert((pointer == nullptr || (type.GetTypeTraits() & trait_flags::cIS_DATA_TYPE)) && "Only data types are valid");
  }
  template <typename T>
  tTypedPointer(T* object) : data(object), type(tDataType<T>())
  {}

  inline operator const tTypedConstPointer&() const
  {
    return reinterpret_cast<const tTypedConstPointer&>(*this);
  }

  inline operator bool() const
  {
    return data != nullptr;
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
  inline void DeepCopyFrom(const tTypedConstPointer& source) const
  {
    assert((source.GetType().GetUnderlyingType() == this->GetType().GetUnderlyingType()) && this->GetRawDataPointer() && source.GetRawDataPointer() && "Types must match and pointers must not be null");
    if (source.GetRawDataPointer() != this->GetRawDataPointer())
    {
      if (type.GetTypeTraits() & trait_flags::cSUPPORTS_BITWISE_COPY)
      {
        memcpy(this->GetRawDataPointer(), source.GetRawDataPointer(), type.GetSize());
      }
      else
      {
        (*type.GetBinaryOperations().deep_copy)(source, *this);
      }
    }
  }

  /*!
   * Deserialize data from input stream
   *
   * \param stream Input stream
   * \throws Throws std::exception on invalid data in stream
   */
  inline void Deserialize(serialization::tInputStream& stream) const
  {
    if (data && (type.GetTypeTraits() & trait_flags::cIS_BINARY_SERIALIZABLE))
    {
      (*type.GetBinarySerialization().deserialize)(stream, *this);
    }
    else
    {
      throw std::runtime_error("Deserialization not supported");
    }
  }

  /*!
   * Deserialize data from input stream
   *
   * \param stream Input stream
   * \throws Throws std::exception on invalid data in stream
   */
  void Deserialize(serialization::tStringInputStream& stream) const;

  /*!
   * Deserialize data from XML node
   *
   * \param node XML node
   * \throws Throws std::exception on invalid data in XML node
   */
  void Deserialize(const xml::tNode& node) const;

  /*!
   * Deserialize data from binary input stream - possibly using non-binary encoding.
   *
   * \param stream Binary input stream
   * \param encoding Encoding to use
   */
  void Deserialize(serialization::tInputStream& stream, serialization::tDataEncoding encoding) const;

  /*!
   * Destruct object that this pointer points to.
   * After this operation, pointer is nullptr.
   * If already nullptr, nothing is done.
   */
  inline void Destruct()
  {
    type.DestructInstance(data);
    data = nullptr;
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
  inline T* Get() const
  {
    assert(typeid(typename NormalizedType<T>::type).name() == type.GetRttiName());
    return static_cast<T*>(data);
  }

  /*!
   * \return Raw void pointer to wrapped object
   */
  inline void* GetRawDataPointer() const
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
   * \return Wrapped object (no type checks via assertions are performed)
   */
  template <typename T>
  inline T* GetUnchecked() const
  {
    return static_cast<T*>(data);
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
  inline tTypedPointer GetVectorElement(size_t index) const
  {
    if (type.IsListType())
    {
      tTypedConstPointer temp = (*type.GetBinaryOperationsVector().get_vector_element)(*this, index);
      return tTypedPointer(const_cast<void*>(temp.GetRawDataPointer()), temp.GetType());
    }
    else if (index >= 1)
    {
      throw std::invalid_argument("Invalid index");
    }
    return *this;
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
  void ResizeVector(size_t new_size) const
  {
    if (type.IsListType())
    {
      (*type.GetBinaryOperationsVector().resize_vector)(*this, new_size);
    }
    else if (new_size != 1)
    {
      throw std::invalid_argument("Invalid size - object is not a std::vector");
    }
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

//----------------------------------------------------------------------
// Private methods and fields
//----------------------------------------------------------------------
private:

  /*! Raw pointer to data/object */
  void* data;

  /*! Runtime type information for the data/object that 'data' points to */
  tType type;

};


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}

#include "rrlib/rtti/tGenericObject.h"

#endif
