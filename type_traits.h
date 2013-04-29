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

#ifndef __rrlib__rtti__type_traits_h__
#define __rrlib__rtti__type_traits_h__

#include <type_traits>

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
#include "rrlib/serialization/type_traits.h"
#endif

namespace rrlib
{
namespace rtti
{
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

}

/*!
 * \author Max Reichardt
 *
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
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
    (std::has_trivial_copy_assign<T>::value ? trait_flags::cHAS_TRIVIAL_ASSIGN : 0) |
#else
    (std::has_trivial_assign<T>::value ? trait_flags::cHAS_TRIVIAL_ASSIGN : 0) |
#endif
    (std::has_trivial_copy_constructor<T>::value ? trait_flags::cHAS_TRIVIAL_COPY_CONSTRUCTOR : 0) |
    (std::has_trivial_destructor<T>::value ? trait_flags::cHAS_TRIVIAL_DESTRUCTOR : 0) |
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
    (std::is_unsigned<T>::value ? trait_flags::cIS_UNSIGNED : 0)
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
    | (serialization::tIsBinarySerializable<T>::value ? trait_flags::cIS_BINARY_SERIALIZABLE : 0) |
    (serialization::tIsStringSerializable<T>::value ? trait_flags::cIS_STRING_SERIALIZABLE : 0) |
    (serialization::tIsXMLSerializable<T>::value ? trait_flags::cIS_XML_SERIALIZABLE : 0)
#endif
    ;
};

/*!
 * This type-trait is used to determine whether a type supports operator '<' .
 */
template <typename T>
struct tHasLessThanOperator
{
  template < typename U >
  static U &Make();

  template < typename U = T >
  static int16_t Test(decltype(Make<U>() < Make<U>()));

  static int32_t Test(...);

  enum { value = sizeof(Test(true)) == sizeof(int16_t) };
};

/*!
 * This type-trait is used to determine whether a type supports operator '==' .
 */
template <typename T>
struct tHasEqualToOperator
{
  template < typename U >
  static U &Make();

  template < typename U = T >
  static int16_t Test(decltype(Make<U>() == Make<U>()));

  static int32_t Test(...);

  enum { value = sizeof(Test(true)) == sizeof(int16_t) };
};


} // namespace
} // namespace

#endif // __rrlib__rtti__tTypeTraitsVector_h__
