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
 * Further type traits related to generic operations are defined in generic_operations.h
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
#include "rrlib/rtti/generic_operations.h"
#include "rrlib/rtti/detail/type_traits.h"
#include "rrlib/rtti/detail/tTypeInfo.h"

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

typedef util::tManagedConstCharPointer(*tGetTypenameFunction)(const tType& type);
typedef std::vector<util::tManagedConstCharPointer>(*tGetTypenamesFunction)(const tType& type);

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
 * (2) identical DeepCopy() & Equals()  (see GenericOperations below)
 * (3) (1) and (2) are also true for std::vector<T> and std::vector<type>
 * (4) Casting from T to 'type' is possible (option will be offered by rrlib_rtti_conversion) and could safely be done with a reinterpret_cast<type&>.
 *
 * This is e.g. true for some wrapper classes (e.g. in rrlib_si_units) - and allows for major optimizations in rrlib_rrti type handling.
 * (With respect to enums, std::underlying_type has similarities to the trait defined here.)
 *
 * In this sense, enum and signed integral types can be seen as having unsigned integral types as underlying types - which is defined below.
 * The template needs to be specialized to support further types.
 *
 * Note: Operations (1), (2) and possibly binary serialization need not be defined for type T in order to be usable with rrlib_rtti - only for type 'type'.
 *       Binary serialization & deserialization needs to be defined for T if cBINARY_SERIALIZATION_DIFFERS is set.
 */
template <typename T>
struct UnderlyingType
{
  /*! Does not need to be defined by specializations */
  enum tEnum { cENUM_TYPE = std::is_enum<T>::value };
  typedef typename std::conditional<cENUM_TYPE, typename std::underlying_type<typename std::conditional<cENUM_TYPE, T, tEnum>::type>::type, T>::type tNonEnum;
  enum { cSIGNED_INTEGRAL_TYPE = std::is_integral<tNonEnum>::value && std::is_signed<tNonEnum>::value };
  typedef typename std::conditional<cSIGNED_INTEGRAL_TYPE, typename std::make_unsigned<typename std::conditional<cSIGNED_INTEGRAL_TYPE, tNonEnum, int>::type>::type, tNonEnum>::type tUnsigned;

  /*! underlying type; is set to T if T does not wrap another type as specified in trait specification */
  typedef tUnsigned type;

  /*!
   * True if 'type' can safely be converted to 'T' using a reinterpret_cast.
   * This should not be set to true if e.g. the constructor of T checks or limits values.
   */
  enum { cREVERSE_CAST_VALID = cSIGNED_INTEGRAL_TYPE || std::is_same<T, type>::value };

  /*! True if types T and 'type' have different binary serialization */
  enum { cBINARY_SERIALIZATION_DIFFERS = cENUM_TYPE };

  /*! True if types T and 'type' have different string and/or XML serialization */
  enum { cOTHER_SERIALIZATION_DIFFERS = true };
};

template <typename T>
struct UnderlyingType<std::vector<T>>
{
  typedef std::vector<typename UnderlyingType<T>::type> type;
  enum { cREVERSE_CAST_VALID = UnderlyingType<T>::cREVERSE_CAST_VALID };
  enum { cBINARY_SERIALIZATION_DIFFERS = UnderlyingType<T>::cBINARY_SERIALIZATION_DIFFERS };
  enum { cOTHER_SERIALIZATION_DIFFERS = UnderlyingType<T>::cOTHER_SERIALIZATION_DIFFERS };
};

/*!
 * Type trait that defines whether an object of type T can be safely deep-copied
 * using memcpy and whether equality can be tested using memcmp.
 *
 * Should the default implementation be inadequate for some type T, it needs to be specialized.
 */
template <typename T>
struct SupportsBitwiseCopy
{
  // std::is_trivially_destructible<T> is a heuristic. However, I have never encountered a type where this is invalid.
  enum { value = (std::is_trivially_destructible<T>::value && (!std::has_virtual_destructor<T>::value) && (!std::is_polymorphic<T>::value)) ||
                 std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, SupportsBitwiseCopy<typename UnderlyingType<T>::type>, std::false_type >::type::value
       };

};

/*!
 * Type trait that determines whether std::vector<T> is supported by rrlib_rtti.
 * May need to be specialized if inadequate for some type T.
 */
template <typename T>
struct IsVectorTypeSupported
{
  enum { value = (!serialization::IsSerializableContainer<T>::value) && (std::is_move_constructible<T>::value || std::is_copy_constructible<T>::value || std::is_move_assignable<T>::value || std::is_copy_assignable<T>::value) };
};

/*!
 * Type trait that defines whether an object of default-constructing an object of type T is equivalent
 * to zeroing the memory it occupies.
 *
 * Should the default implementation be inadequate for some type T, it needs to be specialized.
 */
template <typename T>
struct IsDefaultConstructionZeroMemory
{
  enum { value = SupportsBitwiseCopy<T>::value && (!std::is_enum<T>::value) };
};

/*!
 * Type trait that defines the rrlib_rtti name of a type.
 * Template can be specialized for types in order to give them other names
 * (possibly because they are more readable - or to retain backward-compatibility).
 * Notably, a name can also be specified in the tDataType() constructor.
 * This type trait, however, is useful for defining default names for templates.
 *
 * To assign additional secondary type names, value may also be a tGetTypenamesFunction
 */
template <typename T>
struct TypeName
{
  enum { cTYPE_DEFINED_IN_RRLIB_RTTI_WITH_NONSTANDARD_NAME = (std::is_integral<T>::value || std::is_same<T, std::string>::value || std::is_same<T, rrlib::time::tDuration>::value || std::is_same<T, rrlib::time::tTimestamp>::value) && (!std::is_same<T, bool>::value) };

  static constexpr tGetTypenameFunction value = cTYPE_DEFINED_IN_RRLIB_RTTI_WITH_NONSTANDARD_NAME ? &detail::tTypeInfo::GetTypeNameDefinedInRRLibRtti : &detail::tTypeInfo::GetDefaultTypeName;
};

/*!
 * This trait defines which other types should be registered (if they have not been already)
 * when a tDataType<T> object is created.
 *
 * Typically, if e.g. tDataType<int> is created, tDataType<std::vector<int>> is also registered
 */
template <typename T>
struct AutoRegisterRelatedTypes
{
  static void Register()
  {
  }
};


/*! Type trait that returns whether type T is a std::vector */
template <typename T>
struct IsStdVector
{
  enum { value = false };
};
template <typename T>
struct IsStdVector<std::vector<T>>
{
  enum { value = true };
};


namespace trait_flags
{

// Bits for different traits (bytes 2 and 3 are sent to connection partners)
static const int cIS_LIST_TYPE = 1;                          // position 1 - so that (flags & 1) is index in uid list
static const int cSERIALIZATION_FUNCTION_OFFSET_BITS = 0x7E; // offset of serialization operation function pointers (flags & cSERIALIZATION_OFFSET_BITS is offset from tTypeInfo pointer)
static const int cBINARY_OPERATION_FUNCTION_POINTERS = 0x80;

static const int cIS_BINARY_SERIALIZABLE = 1 << 8;
static const int cIS_STRING_SERIALIZABLE = 1 << 9;
static const int cIS_XML_SERIALIZABLE = 1 << 10;
static const int cIS_ENUM = 1 << 11;
static const int cIS_DATA_TYPE = 1 << 12;
static const int cIS_RPC_TYPE = 1 << 13;
static const int cHAS_LIST_TYPE = 1 << 14;

static const int cHAS_UNDERLYING_TYPE = 1 << 15;
static const int cIS_CAST_TO_UNDERLYING_TYPE_IMPLICIT = 1 << 16;
static const int cIS_REINTERPRET_CAST_FROM_UNDERLYING_TYPE_VALID = 1 << 17;
static const int cIS_CAST_FROM_UNDERLYING_TYPE_IMPLICIT = 1 << 18;
static const int cIS_UNDERLYING_TYPE_BINARY_SERIALIZATION_DIFFERENT = 1 << 19;
static const int cSUPPORTS_BITWISE_COPY = 1 << 20;
//static const int cHAS_DIFFERENT_BINARY_SERIALIZATION_THAN_UNDERLYING_TYPE = 1 << 20;

static const int cIS_INTEGRAL = 1 << 21;
static const int cIS_LIST_TYPE_COPY = 1 << 22;  // copy of first flag (so that this info is also transferred to connection partners)
static const int cHAS_TRIVIAL_DESTRUCTOR = 1 << 23;

static const int cHAS_VIRTUAL_DESTRUCTOR = 1 << 24;
static const int cIS_DEFAULT_CONSTRUCTION_ZERO_MEMORY = 1 << 25;
/*static const int cIS_SIGNED = 1 << 23;

static const int cIS_ABSTRACT = 1 << 16;
static const int cIS_ARITHMETIC = 1 << 17;
static const int cIS_ARRAY = 1 << 18;
static const int cIS_CLASS = 1 << 19;
static const int cIS_EMPTY = 1 << 20;
static const int cIS_FLOATING_POINT = 1 << 21;
static const int cIS_OBJECT = 1 << 23;
static const int cIS_POD = 1 << 24;
static const int cIS_POINTER = 1 << 25;
static const int cIS_SCALAR = 1 << 26;
static const int cIS_UNSIGNED = 1 << 28;*/

static_assert((cIS_LIST_TYPE | cIS_DATA_TYPE) == detail::tTypeInfo::cLIST_TRAIT_FLAGS, "Flag inconsistency");

} // namespace

/*!
 * Stores various type traits determined at compile time to bit vector
 * so that traits are available at runtime.
 *
 * (Implementation note: We use constants (no bitset objects etc.) to
 *  ensure everything is calculated at compile time)
 */
template <typename T>
struct TypeTraitsVector
{
  enum { cHAS_DIFFERENT_UNDERLYING_TYPE = !std::is_same<typename UnderlyingType<T>::type, T>::value };
  enum { cSERIALIZATION_FUNCTION_OFFSET = sizeof(detail::tTypeInfo) + (SupportsBitwiseCopy<T>::value && IsDefaultConstructionZeroMemory<T>::value ? 0 : sizeof(tBinaryOperations)) + (IsStdVector<T>::value ? sizeof(tBinaryOperationsVector) : 0) };
  static_assert((cSERIALIZATION_FUNCTION_OFFSET & trait_flags::cSERIALIZATION_FUNCTION_OFFSET_BITS) == cSERIALIZATION_FUNCTION_OFFSET, "Invalid offset");

  // Bit vector for type (the remaining flags are set in the code)
  static const uint32_t value =
    (IsStdVector<T>::value ? (trait_flags::cIS_LIST_TYPE | trait_flags::cIS_LIST_TYPE_COPY) : 0) |
    cSERIALIZATION_FUNCTION_OFFSET | // offset

    (serialization::IsBinarySerializable<T>::value ? trait_flags::cIS_BINARY_SERIALIZABLE : 0) |
    (serialization::IsStringSerializable<T>::value ? trait_flags::cIS_STRING_SERIALIZABLE : 0) |
    (serialization::IsXMLSerializable<T>::value ? trait_flags::cIS_XML_SERIALIZABLE : 0) |

    (std::is_enum<T>::value ? trait_flags::cIS_ENUM : 0) |
    (IsVectorTypeSupported<T>::value ? trait_flags::cHAS_LIST_TYPE : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE ? trait_flags::cHAS_UNDERLYING_TYPE : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE && IsImplicitlyConvertible<T, typename UnderlyingType<T>::type>::value ? trait_flags::cIS_CAST_TO_UNDERLYING_TYPE_IMPLICIT : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE && UnderlyingType<T>::cREVERSE_CAST_VALID ? trait_flags::cIS_REINTERPRET_CAST_FROM_UNDERLYING_TYPE_VALID : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE && IsImplicitlyConvertible<typename UnderlyingType<T>::type, T>::value ? trait_flags::cIS_CAST_FROM_UNDERLYING_TYPE_IMPLICIT : 0) |
    (cHAS_DIFFERENT_UNDERLYING_TYPE && UnderlyingType<T>::cBINARY_SERIALIZATION_DIFFERS ? trait_flags::cIS_UNDERLYING_TYPE_BINARY_SERIALIZATION_DIFFERENT : 0) |
    (SupportsBitwiseCopy<T>::value ? trait_flags::cSUPPORTS_BITWISE_COPY : 0) |

    (std::has_virtual_destructor<T>::value ? trait_flags::cHAS_VIRTUAL_DESTRUCTOR : 0) |
    (std::is_trivially_destructible<T>::value ? trait_flags::cHAS_TRIVIAL_DESTRUCTOR : 0) |
    (IsDefaultConstructionZeroMemory<T>::value ? trait_flags::cIS_DEFAULT_CONSTRUCTION_ZERO_MEMORY : 0) |
    (std::is_integral<T>::value ? trait_flags::cIS_INTEGRAL : 0)

    /*(std::is_abstract<T>::value ? trait_flags::cIS_ABSTRACT : 0) |
    (std::is_arithmetic<T>::value ? trait_flags::cIS_ARITHMETIC : 0) |
    (std::is_array<T>::value ? trait_flags::cIS_ARRAY : 0) |
    (std::is_class<T>::value ? trait_flags::cIS_CLASS : 0) |
    (std::is_empty<T>::value ? trait_flags::cIS_EMPTY : 0) |
    (std::is_floating_point<T>::value ? trait_flags::cIS_FLOATING_POINT : 0) |
    (std::is_integral<T>::value ? trait_flags::cIS_INTEGRAL : 0) |
    (std::is_object<T>::value ? trait_flags::cIS_OBJECT : 0) |
    (std::is_pod<T>::value ? trait_flags::cIS_POD : 0) |
    (std::is_pointer<T>::value ? trait_flags::cIS_POINTER : 0) |
    (std::is_scalar<T>::value ? trait_flags::cIS_SCALAR : 0) |
    (std::is_signed<T>::value ? trait_flags::cIS_SIGNED : 0) |
    (std::is_unsigned<T>::value ? trait_flags::cIS_UNSIGNED : 0) |
    (std::is_trivially_destructible<T>::value ? trait_flags::cHAS_TRIVIAL_DESTRUCTOR : 0) |*/
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
template <typename T>
struct HasEqualToOperator<std::vector<T>>
{
  enum { value = HasEqualToOperator<T>::value };
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

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}

static_assert(std::is_same<typename rrlib::rtti::NormalizedType<bool>::type, bool>::value, "Invalid trait implementation");
static_assert(std::is_same<typename rrlib::rtti::NormalizedType<unsigned long>::type, typename std::conditional<sizeof(unsigned long) == 8, unsigned long long, unsigned int>::type>::value, "Invalid trait implementation");

#endif
