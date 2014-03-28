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
/*!\file    rrlib/rtti/detail/generic_operations.h
 *
 * \author  Max Reichardt
 *
 * \date    2014-03-27
 *
 * Implementation of default generic operations for types
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__generic_operations_h__
#define __rrlib__rtti__detail__generic_operations_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

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
template <typename T>
struct HasEqualToOperator;
template <typename T>
struct SupportsBitwiseCopy;

namespace detail
{

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

/*!
 * Type trait to determine whether T has a CopyFrom member function
 */
template <typename T>
struct HasCopyFromMethod
{
  template <typename U>
  static U &Make();

  template <typename U = T>
  static int16_t Test(decltype(Make<U>().CopyFrom(Make<const U>()))*);

  static int32_t Test(...);

  enum { value = sizeof(Test(static_cast<void*>(nullptr))) == sizeof(int16_t) }; // g++ 4.8 requires this cast somehow
};

template < typename T,
         bool COPY_CONSTRUCTIBLE = std::is_copy_constructible<T>::value,
         bool HAS_COPY_FROM = HasCopyFromMethod<T>::value,
         bool BINARY_SERIALIZABLE = serialization::IsBinarySerializable<T>::value,
         bool SUPPORTS_BITWISE_COPY = SupportsBitwiseCopy<T>::value >
struct DeepCopyOperation
{
  // no detected way to create deep copy
};

template <typename T, bool COPY_CONSTRUCTIBLE, bool BINARY_SERIALIZABLE, bool SUPPORTS_BITWISE_COPY>
struct DeepCopyOperation<T, COPY_CONSTRUCTIBLE, true, BINARY_SERIALIZABLE, SUPPORTS_BITWISE_COPY>
{
  static void DeepCopy(const T& source, T& destination)
  {
    destination.CopyFrom(source);
  }
};

template <typename T, bool BINARY_SERIALIZABLE, bool SUPPORTS_BITWISE_COPY>
struct DeepCopyOperation<T, true, false, BINARY_SERIALIZABLE, SUPPORTS_BITWISE_COPY>
{
  static void DeepCopy(const T& source, T& destination)
  {
    destination = source;
  }
};

template <typename T, bool BINARY_SERIALIZABLE>
struct DeepCopyOperation<T, false, false, BINARY_SERIALIZABLE, true>
{
  static void DeepCopy(const T& source, T& destination)
  {
    memcpy(&destination, &source, sizeof(T));
  }
};

template <typename T>
struct DeepCopyOperation<T, false, false, true, false>
{
  static void DeepCopy(const T& source, T& destination)
  {
    serialization::SerializationBasedDeepCopy(source, destination);
  }
};

template < typename T,
         bool EQUALS_OPERATOR = HasEqualToOperator<T>::value,
         bool BINARY_SERIALIZABLE = serialization::IsBinarySerializable<T>::value,
         bool SUPPORTS_BITWISE_COPY = SupportsBitwiseCopy<T>::value >
struct EqualsOperation
{
  // no detected way to compare
};

template <typename T, bool BINARY_SERIALIZABLE, bool SUPPORTS_BITWISE_COPY>
struct EqualsOperation<T, true, BINARY_SERIALIZABLE, SUPPORTS_BITWISE_COPY>
{
  static bool Equals(const T& object1, const T& object2)
  {
    return object1 == object2;
  }
};

template <typename T, bool BINARY_SERIALIZABLE>
struct EqualsOperation<T, false, BINARY_SERIALIZABLE, true>
{
  static bool Equals(const T& object1, const T& object2)
  {
    return memcmp(&object1, &object2, sizeof(T)) == 0;
  }
};

template <typename T>
struct EqualsOperation<T, false, true, false>
{
  static bool Equals(const T& object1, const T& object2)
  {
    return serialization::SerializationEquals(object1, object2);
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
