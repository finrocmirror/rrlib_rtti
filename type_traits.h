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
/*!\file    rrlib/rtti/type_traits.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * Various type traits required for rrlib_rtti.
 * Most of them can be specialized.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__type_traits_h__
#define __rrlib__rtti__type_traits_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <type_traits>
#include "rrlib/serialization/serialization.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tIsListType.h"
#include "rrlib/rtti/detail/generic_operations.h"

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
class tDataType;

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

namespace trait_flags
{

// Bits for different traits (note regarding order: the first four are interesting for other runtime environments in Finroc)
static const int cIS_BINARY_SERIALIZABLE = 1 << 0;
static const int cIS_STRING_SERIALIZABLE = 1 << 1;
static const int cIS_XML_SERIALIZABLE = 1 << 2;
static const int cIS_ENUM = 1 << 3;
static const int cHAS_TRIVIAL_ASSIGN = 1 << 4;
static const int cHAS_TRIVIAL_COPY_CONSTRUCTOR = 1 << 5;
static const int cHAS_TRIVIAL_DESTRUCTOR = 1 << 6;
static const int cHAS_VIRTUAL_DESTRUCTOR = 1 << 7;
static const int cIS_ABSTRACT = 1 << 8;
static const int cIS_ARITHMETIC = 1 << 9;
static const int cIS_ARRAY = 1 << 10;
static const int cIS_CLASS = 1 << 11;
static const int cIS_EMPTY = 1 << 12;
static const int cIS_FLOATING_POINT = 1 << 13;
static const int cIS_INTEGRAL = 1 << 14;
static const int cIS_OBJECT = 1 << 15;
static const int cIS_POD = 1 << 16;
static const int cIS_POINTER = 1 << 17;
static const int cIS_SCALAR = 1 << 18;
static const int cIS_SIGNED = 1 << 19;
static const int cIS_UNSIGNED = 1 << 20;

} // namespace

/*!
 * Stores various type traits determined at compile time to bit vector
 * so that traits are available at runtime.
 *
 * (Implementation note: We use constants (no bitset objects etc.) to
 *  ensure everything is calculated at compile time)
 */
template <typename T>
struct tTypeTraitsVector
{

  // Bit vector for type
  static const int value =
    (std::has_trivial_copy_assign<T>::value ? trait_flags::cHAS_TRIVIAL_ASSIGN : 0) |
    (std::has_trivial_copy_constructor<T>::value ? trait_flags::cHAS_TRIVIAL_COPY_CONSTRUCTOR : 0) |
    (std::is_trivially_destructible<T>::value ? trait_flags::cHAS_TRIVIAL_DESTRUCTOR : 0) |
    (std::is_trivially_destructible<T>::value ? trait_flags::cHAS_VIRTUAL_DESTRUCTOR : 0) |
    (std::is_abstract<T>::value ? trait_flags::cIS_ABSTRACT : 0) |
    (std::is_arithmetic<T>::value ? trait_flags::cIS_ARITHMETIC : 0) |
    (std::is_array<T>::value ? trait_flags::cIS_ARRAY : 0) |
    (std::is_class<T>::value ? trait_flags::cIS_CLASS : 0) |
    (std::is_empty<T>::value ? trait_flags::cIS_EMPTY : 0) |
    (std::is_enum<T>::value ? trait_flags::cIS_ENUM : 0) |
    (std::is_floating_point<T>::value ? trait_flags::cIS_FLOATING_POINT : 0) |
    (std::is_integral<T>::value ? trait_flags::cIS_INTEGRAL : 0) |
    (std::is_object<T>::value ? trait_flags::cIS_OBJECT : 0) |
    (std::is_pod<T>::value ? trait_flags::cIS_POD : 0) |
    (std::is_pointer<T>::value ? trait_flags::cIS_POINTER : 0) |
    (std::is_scalar<T>::value ? trait_flags::cIS_SCALAR : 0) |
    (std::is_signed<T>::value ? trait_flags::cIS_SIGNED : 0) |
    (std::is_unsigned<T>::value ? trait_flags::cIS_UNSIGNED : 0)
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
    | (serialization::IsBinarySerializable<T>::value ? trait_flags::cIS_BINARY_SERIALIZABLE : 0) |
    (serialization::IsStringSerializable<T>::value ? trait_flags::cIS_STRING_SERIALIZABLE : 0) |
    (serialization::IsXMLSerializable<T>::value ? trait_flags::cIS_XML_SERIALIZABLE : 0)
#endif
    ;
};

/*!
 * This type trait is used to determine whether a type supports operator '<' .
 */
template <typename T>
struct HasLessThanOperator
{
  template <typename U>
  static U &Make();

  template <typename U = T>
  static int16_t Test(decltype(Make<U>() < Make<U>()));

  static int32_t Test(...);

  enum { value = sizeof(Test(true)) == sizeof(int16_t) };
};

/*!
 * This type trait is used to determine whether a type supports operator '==' .
 */
template <typename T>
struct HasEqualToOperator
{
  template <typename U>
  static U &Make();

  template <typename U = T>
  static int16_t Test(decltype(Make<U>() == Make<U>()));

  static int32_t Test(...);

  enum { value = sizeof(Test(true)) == sizeof(int16_t) };
};

/*!
 * Type trait that defines whether an object of type T can be safely deep-copied
 * using memcpy and whether equality can be tested using memcmp.
 */
template <typename T>
struct SupportsBitwiseCopy
{
  // std::is_trivially_destructible<T> is a heuristic. However, I have never encountered a type where this is invalid.
  enum { value = std::is_trivially_destructible<T>::value && (!std::has_virtual_destructor<T>::value) };
};

/*!
 * This type trait defines various generic operations for objects of a type T.
 * tGenericObject provides all these operations at runtime
 * (with only runtime type information available)
 *
 * This trait may be specialized in order to use better (e.g. faster, accurate) operations for T.
 */
template <typename T>
struct GenericOperations;

/*!
 * Base class with default implementation.
 * Is a base class so that specializations may conveniently reuse parts of it.
 */
template <typename T, bool IS_CONTAINER = serialization::IsSerializableContainer<T>::value>
struct GenericOperationsDefault
{
  /*!
   * Create a deep copy of an object.
   * A deep copy means that the destination object must not
   * change if the source object is modified or deleted.
   * Serialization of source and destination objects are equal after calling this.
   *
   * \param source Object to be copied
   * \param destination Object to copy source object to
   */
  static void DeepCopy(const T& source, T& destination)
  {
    detail::DeepCopyOperation<T>::DeepCopy(source, destination);
  }

  /*!
   * Returns whether two objects are equal.
   * If T supports the '==' operator, this is typically used for the Equals operation.
   * Serializing equal objects produces identical data.
   *
   * \param object1 First object to compare
   * \param object2 Second object to compare
   */
  static bool Equals(const T& object1, const T& object2)
  {
    return detail::EqualsOperation<T>::Equals(object1, object2);
  }
};

/*!
 * Default implementation for STL containers with elements of type T
 */
template <typename T, bool MAP, bool SIMPLE = std::is_fundamental<T>::value>
struct GenericOperationsContainer
{
  template <typename TContainer>
  static void DeepCopy(const TContainer& source, TContainer& destination)
  {
    serialization::ContainerResize<T>::Resize(destination, source.size());
    auto dest_it = destination.begin();
    for (auto src_it = source.begin(); src_it != source.end(); ++src_it, ++dest_it)
    {
      GenericOperations<T>::DeepCopy(*src_it, *dest_it);
    }
  }

  template <typename TContainer>
  static bool EqualsImplementation(const TContainer& object1, const TContainer& object2)
  {
    return object1.size() == object2.size() && std::equal(object1.begin(), object1.end(), object2.begin(), &GenericOperations<T>::Equals);
  }
};

template <typename T, bool MAP>
struct GenericOperationsContainer<T, MAP, true>
{
  template <typename TContainer>
  static void DeepCopy(const TContainer& source, TContainer& destination)
  {
    destination = source;
  }

  template <typename TContainer>
  static bool EqualsImplementation(const TContainer& object1, const TContainer& object2)
  {
    return object1 == object2;
  }
};

// Map
template <typename T>
struct GenericOperationsContainer<T, true, false>
{
  template <typename TMap>
  static void DeepCopy(const TMap& source, TMap& destination)
  {
    destination.clear();
    for (auto it = source.begin(); it != source.end(); ++it)
    {
      typedef typename TMap::key_type tKey;
      typedef typename TMap::mapped_type tMapped;
      std::pair<tKey, tMapped> entry(it->first, serialization::DefaultInstantiation<tMapped>::Create());
      GenericOperations<tMapped>::DeepCopy(it->second, entry.second);
      destination.insert(std::move(entry));
    }
  }

  template <typename TContainer>
  static bool EqualsImplementation(const TContainer& object1, const TContainer& object2)
  {
    return object1.size() == object2.size() && std::equal(object1.begin(), object1.end(), object2.begin(), &GenericOperations<T>::Equals);
  }
};

template <typename T>
struct GenericOperationsDefault<T, true> : GenericOperationsContainer<typename T::value_type, serialization::IsSerializableMap<T>::value>
{
  typedef GenericOperationsContainer<typename T::value_type, serialization::IsSerializableMap<T>::value> tBase;

  // we need this non-template 'Equals' function to get a function pointer on 'Equals' at other places
  static inline bool Equals(const T& object1, const T& object2)
  {
    return tBase::EqualsImplementation(object1, object2);
  }
};

template <typename T>
struct GenericOperations : GenericOperationsDefault<T>
{
};

template <>
struct GenericOperations<std::string> : GenericOperationsDefault<std::string, false>
{
};

/*!
 * This trait defines which other types should be registered (if they have not been already)
 * when a tDataType<T> object is created.
 *
 * Typically, if e.g. tDataType<int> is created, tDataType<std::vector<int>> is also registered
 */
template < typename T,
         bool REGISTER_LIST_TYPE = (!serialization::IsSerializableContainer<T>::value) &&
         (!std::is_base_of<tIsListType<false, false>, T>::value) && (!std::is_base_of<tIsListType<false, true>, T>::value) >
struct AutoRegisterRelatedTypes
{
  static void Register()
  {
    tDataType<std::vector<T>>();
  }
};

template <typename T>
struct AutoRegisterRelatedTypes<T, false>
{
  static void Register()
  {
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
