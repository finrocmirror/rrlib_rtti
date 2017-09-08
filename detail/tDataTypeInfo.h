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
/*!\file    rrlib/rtti/detail/tDataTypeInfo.h
 *
 * \author  Max Reichardt
 *
 * \date    2016-07-23
 *
 * \brief   Contains tDataTypeInfo
 *
 * \b tDataTypeInfo
 *
 * Type info for a specific data type T - provided mostly as compile-time constant
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__tDataTypeInfo_h__
#define __rrlib__rtti__detail__tDataTypeInfo_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/detail/tTypeInfo.h"
#include "rrlib/rtti/type_traits.h"
#include "rrlib/rtti/detail/generic_operations.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace rtti
{
namespace detail
{

enum tTableLayoutFlags
{
  eTLF_BINARY_OPS = 1,
  eTLF_BINARY_SERIALIZATION = 2,
  eTLF_OTHER_SERIALIZATION = 4,
  eTLF_VECTOR_TYPE = 8,
  eTLF_ENUM = 16,
};

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
template <typename T>
struct TableLayoutFlags
{
  enum { value = std::is_enum<T>::value ? eTLF_ENUM :
                 ((IsStdVector<T>::value ? eTLF_VECTOR_TYPE : 0) |
                  (!(SupportsBitwiseCopy<T>::value && IsDefaultConstructionZeroMemory<T>::value) ? eTLF_BINARY_OPS : 0) |
                  (serialization::IsBinarySerializable<T>::value ? eTLF_BINARY_SERIALIZATION : 0) |
                  ((serialization::IsXMLSerializable<T>::value || serialization::IsStringSerializable<T>::value) ? eTLF_OTHER_SERIALIZATION : 0))
       };
};

struct Empty
{};

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Data Type Info
/*!
 * Type info for a specific data type T - provided mostly as compile-time constant
 */
template <typename T, unsigned int Tflags = TableLayoutFlags<T>::value>
struct tDataTypeInfo
{
  static_assert(!std::is_same<T, T>::value, "This should not be instantiated");
  static tTypeInfo::tSharedInfo shared_info;
  struct tTable
  {};
  static constexpr tTable value = {};
};

template <typename T>
struct tDataTypeInfo<T, 0>
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)) // Type info
  };
};

template <typename T>
struct tDataTypeInfo<T, eTLF_BINARY_OPS>
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value } // Binary operations
  };
};

template <typename T>
struct tDataTypeInfo<T, eTLF_BINARY_SERIALIZATION>
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinarySerializationOperations binary_serialization_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::DeserializeFromBinaryFunction<T>::value, rtti::SerializeToBinaryFunction<T>::value } // Binary serialization
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_BINARY_SERIALIZATION | eTLF_BINARY_OPS >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations;
    tBinarySerializationOperations binary_serialization_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Binary operations
    { rtti::DeserializeFromBinaryFunction<T>::value, rtti::SerializeToBinaryFunction<T>::value } // Binary serialization
  };
};

template <typename T>
struct tDataTypeInfo<T, eTLF_OTHER_SERIALIZATION>
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    typename std::conditional<serialization::IsStringSerializable<T>::value, tStringSerializationOperations, tXMLSerializationOperations>::type other_serialization_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { std::conditional<serialization::IsStringSerializable<T>::value, rtti::DeserializeFromStringFunction<T>, rtti::DeserializeFromXMLFunction<T>>::type::value, std::conditional<serialization::IsStringSerializable<T>::value, rtti::SerializeToStringFunction<T>, rtti::SerializeToXMLFunction<T>>::type::value } // Other serialization
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_OTHER_SERIALIZATION | eTLF_BINARY_OPS >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations;
    typename std::conditional<serialization::IsStringSerializable<T>::value, tStringSerializationOperations, tXMLSerializationOperations>::type other_serialization_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Binary operations
    { std::conditional<serialization::IsStringSerializable<T>::value, rtti::DeserializeFromStringFunction<T>, rtti::DeserializeFromXMLFunction<T>>::type::value, std::conditional<serialization::IsStringSerializable<T>::value, rtti::SerializeToStringFunction<T>, rtti::SerializeToXMLFunction<T>>::type::value } // Other serialization
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_OTHER_SERIALIZATION | eTLF_BINARY_SERIALIZATION >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinarySerializationOperations binary_serialization_operations;
    typename std::conditional<serialization::IsStringSerializable<T>::value, tStringSerializationOperations, tXMLSerializationOperations>::type other_serialization_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::DeserializeFromBinaryFunction<T>::value, rtti::SerializeToBinaryFunction<T>::value }, // Binary serialization
    { std::conditional<serialization::IsStringSerializable<T>::value, rtti::DeserializeFromStringFunction<T>, rtti::DeserializeFromXMLFunction<T>>::type::value, std::conditional<serialization::IsStringSerializable<T>::value, rtti::SerializeToStringFunction<T>, rtti::SerializeToXMLFunction<T>>::type::value } // Other serialization
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_OTHER_SERIALIZATION | eTLF_BINARY_SERIALIZATION | eTLF_BINARY_OPS >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations;
    tBinarySerializationOperations binary_serialization_operations;
    typename std::conditional<serialization::IsStringSerializable<T>::value, tStringSerializationOperations, tXMLSerializationOperations>::type other_serialization_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Binary operations
    { rtti::DeserializeFromBinaryFunction<T>::value, rtti::SerializeToBinaryFunction<T>::value }, // Binary serialization
    { std::conditional<serialization::IsStringSerializable<T>::value, rtti::DeserializeFromStringFunction<T>, rtti::DeserializeFromXMLFunction<T>>::type::value, std::conditional<serialization::IsStringSerializable<T>::value, rtti::SerializeToStringFunction<T>, rtti::SerializeToXMLFunction<T>>::type::value } // Other serialization
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_VECTOR_TYPE | eTLF_BINARY_OPS >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations_list_1;
    tBinaryOperationsVector binary_operations_list_2;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Vector binary operations 1
    { rtti::GetVectorElementFunction<T>::value, rtti::GetVectorSizeFunction<T>::value, rtti::ResizeVectorFunction<T>::value } // Vectory binary operations 2
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_VECTOR_TYPE | eTLF_BINARY_SERIALIZATION | eTLF_BINARY_OPS >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations_list_1;
    tBinaryOperationsVector binary_operations_list_2;
    tBinarySerializationOperations binary_serialization_operations_list;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Vector binary operations 1
    { rtti::GetVectorElementFunction<T>::value, rtti::GetVectorSizeFunction<T>::value, rtti::ResizeVectorFunction<T>::value }, // Vectory binary operations 2
    { rtti::DeserializeFromBinaryFunction<T>::value, rtti::SerializeToBinaryFunction<T>::value } // Binary serialization
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_VECTOR_TYPE | eTLF_OTHER_SERIALIZATION | eTLF_BINARY_OPS >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations_list_1;
    tBinaryOperationsVector binary_operations_list_2;
    tXMLSerializationOperations xml_serialization_operations_list;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Vector binary operations 1
    { rtti::GetVectorElementFunction<T>::value, rtti::GetVectorSizeFunction<T>::value, rtti::ResizeVectorFunction<T>::value }, // Vectory binary operations 2
    { rtti::DeserializeFromXMLFunction<T>::value, rtti::SerializeToXMLFunction<T>::value } // XML serialization
  };
};

template <typename T>
struct tDataTypeInfo < T, eTLF_VECTOR_TYPE | eTLF_OTHER_SERIALIZATION | eTLF_BINARY_SERIALIZATION | eTLF_BINARY_OPS >
{
  static tTypeInfo::tSharedInfo shared_info;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations_list_1;
    tBinaryOperationsVector binary_operations_list_2;
    tBinarySerializationOperations binary_serialization_operations_list;
    tXMLSerializationOperations xml_serialization_operations_list;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Vector binary operations 1
    { rtti::GetVectorElementFunction<T>::value, rtti::GetVectorSizeFunction<T>::value, rtti::ResizeVectorFunction<T>::value }, // Vectory binary operations 2
    { rtti::DeserializeFromBinaryFunction<T>::value, rtti::SerializeToBinaryFunction<T>::value }, // Binary serialization
    { rtti::DeserializeFromXMLFunction<T>::value, rtti::SerializeToXMLFunction<T>::value } // XML serialization
  };
};

template <typename T>
struct tDataTypeInfo<T, eTLF_ENUM>
{
  static tTypeInfo::tSharedInfoEnum shared_info;
  static_assert(SupportsBitwiseCopy<T>::value, "Enums must support bitwise copy");
  typedef std::vector<T> V;
  typedef typename NormalizedType<typename ElementType<T>::type>::type tElement;
  typedef typename std::conditional<std::is_same<typename UnderlyingType<T>::type, T>::value, void, typename UnderlyingType<T>::type>::type tUnderlying;
  struct tTable
  {
    tTypeInfo data_type_info;
    tBinaryOperations binary_operations;
    tBinarySerializationOperations binary_serialization_operations;
    typename std::conditional<serialization::IsStringSerializable<T>::value, tStringSerializationOperations, tXMLSerializationOperations>::type other_serialization_operations;
  } static constexpr value =
  {
    tTypeInfo(typeid(T), TypeTraitsVector<T>::value | trait_flags::cIS_DATA_TYPE, std::is_same<void, tUnderlying>::value ? nullptr : &tDataTypeInfo<tUnderlying>::value.data_type_info, std::is_same<tElement, void>::value ? &tTypeInfo::cNULL_TYPE_INFO : &tDataTypeInfo<tElement>::value.data_type_info, &shared_info, sizeof(T)), // Type info
    { rtti::ConstructorFunction<T>::value, rtti::DestructorFunction<T>::value, rtti::DeepCopyFunction<T>::value, EqualsFunction<T>::value }, // Binary operations
    { rtti::DeserializeFromBinaryFunction<T>::value, rtti::SerializeToBinaryFunction<T>::value }, // Binary serialization
    { std::conditional<serialization::IsStringSerializable<T>::value, rtti::DeserializeFromStringFunction<T>, rtti::DeserializeFromXMLFunction<T>>::type::value, std::conditional<serialization::IsStringSerializable<T>::value, rtti::SerializeToStringFunction<T>, rtti::SerializeToXMLFunction<T>>::type::value } // Other serialization
  };
};

enum { cVOID_FLAGS = TableLayoutFlags<void>::value };

template <>
struct tDataTypeInfo<void, cVOID_FLAGS>
{
  struct tTable
  {
    tTypeInfo data_type_info;
  } static constexpr value =
  {
    { typeid(void), 0, nullptr, nullptr, nullptr, 0 }, // Type info
  };
};

template <unsigned int Tflags>
struct ListTypeOffsetInTable
{
  enum { value = eTLF_ENUM ? 8 : (4 + (Tflags & eTLF_BINARY_OPS) ? 3 : 0 + (Tflags & eTLF_BINARY_SERIALIZATION) ? 2 : 0 + (Tflags & eTLF_OTHER_SERIALIZATION) ? 2 : 0) };
};

template <typename T, unsigned int Tflags> constexpr typename tDataTypeInfo<T, Tflags>::tTable tDataTypeInfo<T, Tflags>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 0>::tTable tDataTypeInfo<T, 0>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 1>::tTable tDataTypeInfo<T, 1>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 2>::tTable tDataTypeInfo<T, 2>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 3>::tTable tDataTypeInfo<T, 3>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 4>::tTable tDataTypeInfo<T, 4>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 5>::tTable tDataTypeInfo<T, 5>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 6>::tTable tDataTypeInfo<T, 6>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 7>::tTable tDataTypeInfo<T, 7>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 9>::tTable tDataTypeInfo<T, 9>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 11>::tTable tDataTypeInfo<T, 11>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 13>::tTable tDataTypeInfo<T, 13>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 15>::tTable tDataTypeInfo<T, 15>::value;
template <typename T> constexpr typename tDataTypeInfo<T, 16>::tTable tDataTypeInfo<T, 16>::value;

template <typename T, unsigned int Tflags> tTypeInfo::tSharedInfo tDataTypeInfo<T, Tflags>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value);
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 0>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 1>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 2>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 3>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 4>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 5>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 6>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 7>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, AutoRegisterVectorType<T>::value ? &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info : &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 9>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 11>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 13>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());
template <typename T> tTypeInfo::tSharedInfo tDataTypeInfo<T, 15>::shared_info(&tDataTypeInfo<T>::value.data_type_info, TypeName<T>::value, &tTypeInfo::cNULL_TYPE_INFO, AutoRegister<T>::Register());

template <typename T>
tTypeInfo::tSharedInfoEnum tDataTypeInfo<T, eTLF_ENUM>::shared_info(&tDataTypeInfo<T>::value.data_type_info,
    TypeName<T>::value,
    make_builder::internal::GetEnumStrings<T>(),
    &tDataTypeInfo<typename std::conditional<AutoRegisterVectorType<T>::value, std::vector<T>, void>::type>::value.data_type_info,
    AutoRegister<T>::Register());


template <typename T>
struct ConstructorFunction<T, false, false, true>
{
  static void Construct(void* placement)
  {
    const make_builder::internal::tEnumStrings& enum_strings = tDataTypeInfo<T>::shared_info.enum_strings;
    if (enum_strings.non_standard_values)
    {
      memcpy(placement, enum_strings.non_standard_values, sizeof(T));
    }
    else
    {
      memset(placement, 0, sizeof(T));
    }
  }

  static constexpr operations::tConstructor value = &Construct;
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
