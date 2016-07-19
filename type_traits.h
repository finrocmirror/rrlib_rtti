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
#include "rrlib/rtti/tType.h"
#include "rrlib/rtti/tIsListType.h"
#include "rrlib/rtti/detail/generic_operations.h"
#include "rrlib/rtti/detail/type_traits.h"

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

/*!
 * Type trait that indicates whether rrlib_rtti should consider the cast from TSource to TDestination to be implicit - e.g. leading to automatic casts in Finroc data ports.
 * Unlike std::is_convertible, this trait should only be true if the cast is lossless for all possible values of TSource:
 *   meaning there are no cases where this could lose precision or values could be outside of TDestination's range.
 *
 * This follows the defensive idea adopted in rrlib_rtti that automatic/implicit casting should never happen for cases where this could be erroneous.
 */
template <typename TSource, typename TDestination>
struct IsImplicitlyConvertible
{
  /*! Enforce explicit casts (for arithmetic types)? */
  enum { cENFORCE_EXPLICIT_CAST = std::is_arithmetic<TSource>::value && std::is_arithmetic<TDestination>::value &&
                                  ((std::is_signed<TSource>::value && std::is_unsigned<TDestination>::value) ||   // sign-loss shall be explicit
                                   (sizeof(TSource) > sizeof(TDestination) || // information-loss shall be explicit
                                    // and also precision-loss
                                    (sizeof(TSource) == sizeof(TDestination) && (std::is_integral<TSource>::value != std::is_integral<TDestination>::value || std::is_signed<TSource>::value == std::is_signed<TDestination>::value))))
       };

  /*! Defines whether cast is implicit in rrlib_rtti */
  enum { value = std::is_convertible<TSource, TDestination>::value && (!cENFORCE_EXPLICIT_CAST) && (!std::is_enum<TSource>::value) && (!std::is_enum<TDestination>::value) };
};

/*!
 * Type trait that can be specialized to indicate that type T and 'type' share the same memory layout and basic binary operations.
 * This means specifically:
 * (1) identical default-construction & destruction
 * (2) identical serialization & deserialization
 * (3) identical DeepCopy() & Equals()  (see GenericOperations below)
 * (4) (1)-(3) are also true for std::vector<T> and std::vector<type>
 * (5) Casting from T to 'type' could safely be done with a reinterpret_cast.
 *
 * This is e.g. true for some wrapper classes (e.g. in rrlib_si_units) - and allows for major optimizations in rrlib_rrti type handling.
 *
 * In this sense, signed integral types can be seen as having unsigned integral types as underlying types - which is defined below.
 *
 * Note: Operations (1) to (3) need not be defined for type T in order to be usable with rrlib_rtti - only for type 'type'
 */
template <typename T>
struct UnderlyingType
{
  /*! Does not need to be defined by specializations */
  enum { cSIGNED_INTEGRAL_TYPE = std::is_integral<T>::value && std::is_signed<T>::value };
  typedef typename std::conditional<cSIGNED_INTEGRAL_TYPE, T, int>::type tIntegralType;

  /*! underlying type; set to T if T does not wrap another type as specified above */
  typedef typename std::conditional<cSIGNED_INTEGRAL_TYPE, typename std::make_unsigned<tIntegralType>::type, T>::type type;

  /*!
   * True if 'type' can safely be converted to 'T' using a reinterpret_cast.
   * This should not be set to true if e.g. the constructor of T checks or limits values.
   */
  enum { reverse_cast_valid = cSIGNED_INTEGRAL_TYPE || std::is_same<T, type>::value };
};

template <typename T>
struct UnderlyingType<std::vector<T>>
{
  typedef std::vector<typename UnderlyingType<T>::type> type;
  enum { reverse_cast_valid = UnderlyingType<T>::reverse_cast_valid };
};

/*!
 * Type trait that defines whether an object of type T can be safely deep-copied
 * using memcpy and whether equality can be tested using memcmp.
 */
template <typename T>
struct SupportsBitwiseCopy
{
  // std::is_trivially_destructible<T> is a heuristic. However, I have never encountered a type where this is invalid.
  enum { value = std::is_trivially_destructible<T>::value && (!std::has_virtual_destructor<T>::value) && (!std::is_polymorphic<T>::value) };
};



namespace trait_flags
{

// Bits for different traits (note regarding order: the first nine are interesting for other runtime environments in Finroc)
static const int cIS_BINARY_SERIALIZABLE = 1 << 0;
static const int cIS_STRING_SERIALIZABLE = 1 << 1;
static const int cIS_XML_SERIALIZABLE = 1 << 2;
static const int cIS_ENUM = 1 << 3;
static const int cUNDERLYING_TYPE = 1 << 4;
static const int cCAST_TO_UNDERLYING_TYPE_IMPLICIT = 1 << 5;
static const int cREINTERPRET_CAST_FROM_UNDERLYING_TYPE_VALID = 1 << 6;
static const int cCAST_FROM_UNDERLYING_TYPE_IMPLICIT = 1 << 7;
static const int cSUPPORTS_BITWISE_COPY = 1 << 8;

static const int cIS_ABSTRACT = 1 << 16;
static const int cIS_ARITHMETIC = 1 << 17;
static const int cIS_ARRAY = 1 << 18;
static const int cIS_CLASS = 1 << 19;
static const int cIS_EMPTY = 1 << 20;
static const int cIS_FLOATING_POINT = 1 << 21;
static const int cIS_INTEGRAL = 1 << 22;
static const int cIS_OBJECT = 1 << 23;
static const int cIS_POD = 1 << 24;
static const int cIS_POINTER = 1 << 25;
static const int cIS_SCALAR = 1 << 26;
static const int cIS_SIGNED = 1 << 27;
static const int cIS_UNSIGNED = 1 << 28;
static const int cHAS_TRIVIAL_DESTRUCTOR = 1 << 29;
static const int cHAS_VIRTUAL_DESTRUCTOR = 1 << 30;

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
  enum { cHAS_DIFFERENT_UNDERLYING_TYPE = !std::is_same<typename UnderlyingType<T>::type, T>::value };

  // Bit vector for type
  static const int value =
    (std::is_trivially_destructible<T>::value ? trait_flags::cHAS_TRIVIAL_DESTRUCTOR : 0) |
    (std::has_virtual_destructor<T>::value ? trait_flags::cHAS_VIRTUAL_DESTRUCTOR : 0) |
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
    (std::is_unsigned<T>::value ? trait_flags::cIS_UNSIGNED : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE ? trait_flags::cUNDERLYING_TYPE : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE && IsImplicitlyConvertible<T, typename UnderlyingType<T>::type>::value ? trait_flags::cCAST_TO_UNDERLYING_TYPE_IMPLICIT : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE && UnderlyingType<T>::reverse_cast_valid ? trait_flags::cREINTERPRET_CAST_FROM_UNDERLYING_TYPE_VALID : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE && IsImplicitlyConvertible<typename UnderlyingType<T>::type, T>::value ? trait_flags::cCAST_FROM_UNDERLYING_TYPE_IMPLICIT : 0) |
    (SupportsBitwiseCopy<T>::value ? trait_flags::cSUPPORTS_BITWISE_COPY : 0)
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
    | (serialization::IsBinarySerializable<T>::value ? trait_flags::cIS_BINARY_SERIALIZABLE : 0) |
    (serialization::IsStringSerializable<T>::value ? trait_flags::cIS_STRING_SERIALIZABLE : 0) |
    (serialization::IsXMLSerializable<T>::value ? trait_flags::cIS_XML_SERIALIZABLE : 0)
#endif
    ;

  // sanity check of type traits for type T
  static_assert((!SupportsBitwiseCopy<T>::value) || (!std::has_virtual_destructor<T>::value), "This would copy/compare vtable pointers");
  static_assert(sizeof(T) == sizeof(typename UnderlyingType<T>::type), "Types need the same memory layout");
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
 * Type trait to get 'normalized' type for type T.
 * It is used to reduce the number of int types to a platform-independent subset.
 * 'type' is usually T - unless this is e.g. a 'long int' or 'char' type.
 */
template <typename T>
struct NormalizedType
{
  typedef typename std::conditional<std::is_integral<T>::value, typename detail::NormalizedIntegerType<sizeof(T), std::is_unsigned<T>::value>::type, T>::type type;
};
template <typename T>
struct NormalizedType<std::vector<T>>
{
  typedef std::vector<typename NormalizedType<T>::type> type;
};
template <>
struct NormalizedType<bool>
{
  typedef bool type;
};

/*!
 * Type trait to determine whether type T is 'normalized'
 */
template <typename T>
struct IsNormalizedType
{
  enum { value = std::is_same<T, typename NormalizedType<T>::type>::value };
};

/*!
 * Type trait that defines the rrlib_rtti name of a type.
 * Template can be specialized for types in order to give them other names
 * (possibly because they are more readable - or to retain backward-compatibility).
 * Notably, a name can also be specified in the tDataType() constructor.
 * This type trait, however, is useful for defining default names for templates.
 */
template <typename T>
struct TypeName
{
  /*!
   * \return Type name to use in rrlib_rtti for type T
   */
  static std::string Get()
  {
    return tType::GetTypeNameFromRtti(typeid(T).name());
  }
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
template <typename T, bool MAP, bool CONST_ELEMENTS, bool SIMPLE = std::is_fundamental<T>::value>
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

template <typename T, bool MAP, bool CONST_ELEMENTS>
struct GenericOperationsContainer<T, MAP, CONST_ELEMENTS, true>
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
template <typename T, bool CONST_ELEMENTS>
struct GenericOperationsContainer<T, true, CONST_ELEMENTS, false>
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

// e.g. Set
template <typename T>
struct GenericOperationsContainer<T, false, true, false>
{
  template <typename TMap>
  static void DeepCopy(const TMap& source, TMap& destination)
  {
    destination.clear();
    for (auto it = source.begin(); it != source.end(); ++it)
    {
      T new_element(serialization::DefaultInstantiation<T>::Create());
      GenericOperations<T>::DeepCopy(*it, new_element);
      destination.emplace(std::move(new_element));
    }
  }

  template <typename TContainer>
  static bool EqualsImplementation(const TContainer& object1, const TContainer& object2)
  {
    return object1.size() == object2.size() && std::equal(object1.begin(), object1.end(), object2.begin(), &GenericOperations<T>::Equals);
  }
};

template <typename T>
struct GenericOperationsDefault<T, true> : GenericOperationsContainer<typename T::value_type, serialization::IsSerializableMap<T>::value, serialization::IsConstElementContainer<T>::value>
{
  typedef GenericOperationsContainer<typename T::value_type, serialization::IsSerializableMap<T>::value, serialization::IsConstElementContainer<T>::value> tBase;

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

static_assert(std::is_same<typename rrlib::rtti::NormalizedType<bool>::type, bool>::value, "Invalid trait implementation");
static_assert(std::is_same<typename rrlib::rtti::NormalizedType<unsigned long>::type, typename std::conditional<sizeof(unsigned long) == 8, unsigned long long, unsigned int>::type>::value, "Invalid trait implementation");

#endif
