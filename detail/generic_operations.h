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
#include "rrlib/rtti/tTypedPointer.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace rtti
{
namespace detail
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------
template < typename T,
         bool Tcopy_constructible = std::is_copy_constructible<T>::value,
         bool Thas_copy_from = HasCopyFromMethod<T>::value,
         bool Tbinary_serializable = serialization::IsBinarySerializable<T>::value,
         bool Tsupports_bitwise_copy = SupportsBitwiseCopy<T>::value >
struct DeepCopyOperationNonContainer
{
  // no detected way to create deep copy
};

template <typename T, bool Tcopy_constructible, bool Tbinary_serializable, bool Tsupports_bitwise_copy>
struct DeepCopyOperationNonContainer<T, Tcopy_constructible, true, Tbinary_serializable, Tsupports_bitwise_copy>
{
  inline static void DeepCopy(const T& source, T& destination)
  {
    destination.CopyFrom(source);
  }
};

template <typename T, bool Tbinary_serializable, bool Tsupports_bitwise_copy>
struct DeepCopyOperationNonContainer<T, true, false, Tbinary_serializable, Tsupports_bitwise_copy>
{
  inline static void DeepCopy(const T& source, T& destination)
  {
    destination = source;
  }
};

template <typename T, bool Tbinary_serializable>
struct DeepCopyOperationNonContainer<T, false, false, Tbinary_serializable, true>
{
  inline static void DeepCopy(const T& source, T& destination)
  {
    memcpy(&destination, &source, sizeof(T));
  }
};

template <typename T>
struct DeepCopyOperationNonContainer<T, false, false, true, false>
{
  inline static void DeepCopy(const T& source, T& destination)
  {
    serialization::SerializationBasedDeepCopy(source, destination);
  }
};

template < typename TContainer,
         typename TElement = typename serialization::IsSerializableContainer<TContainer>::tValue,
         bool Tcopy_constructible = std::is_copy_constructible<TElement>::value,
         bool Tsupports_bitwise_copy = SupportsBitwiseCopy<TElement>::value >
struct DeepCopyOperationContainer
{
  inline static void DeepCopy(const TContainer& source, TContainer& destination)
  {
    serialization::ContainerResize<TElement>::Resize(destination, source.size());
    auto dest_it = destination.begin();
    for (auto src_it = source.begin(); src_it != source.end(); ++src_it, ++dest_it)
    {
      rtti::DeepCopyOperation<TElement>::DeepCopy(*src_it, *dest_it);
    }
  }
};
template <typename TContainer, typename TElement, bool Tsupports_bitwise_copy>
struct DeepCopyOperationContainer<TContainer, TElement, true, Tsupports_bitwise_copy>
{
  inline static void DeepCopy(const TContainer& source, TContainer& destination)
  {
    destination = source;
  }
};
template <typename TElement>
struct DeepCopyOperationContainer<std::vector<TElement>, TElement, false, true>
{
  inline static void DeepCopy(const std::vector<TElement>& source, std::vector<TElement>& destination)
  {
    serialization::ContainerResize<TElement>::Resize(destination, source.size());
    if (source.size())
    {
      memcpy(&destination[0], &source[0], source.size() * sizeof(TElement));
    }
  }
};



template < typename T, bool Tunderlying_type = !std::is_same<typename UnderlyingType<T>::type, T>::value, bool Tconvertible = std::is_convertible<T, typename UnderlyingType<T>::type>::value, bool Tconvertible_normalized = std::is_convertible<T, typename NormalizedType<T>::type>::value >
struct DeepCopyOperation : std::conditional<serialization::IsSerializableContainer<T>::value, detail::DeepCopyOperationContainer<typename NormalizedType<T>::type>, detail::DeepCopyOperationNonContainer<typename NormalizedType<T>::type>>::type
{};
template <typename T> // e.g. for std::vector<size_t> and std::vector<long int>, which is not assignable
struct DeepCopyOperation<T, false, true, false>
{
  inline static void DeepCopy(const T& source, T& destination)
  {
    typedef typename NormalizedType<T>::type tNormalized;
    rtti::DeepCopyOperation<tNormalized>::DeepCopy(reinterpret_cast<const tNormalized&>(source), reinterpret_cast<tNormalized&>(destination));
  }
};
template <typename T, bool Tconvertible_normalized>
struct DeepCopyOperation<T, true, true, Tconvertible_normalized> : rtti::DeepCopyOperation<typename UnderlyingType<T>::type>
{};
template <typename T, bool Tconvertible_normalized>
struct DeepCopyOperation<T, true, false, Tconvertible_normalized>
{
  inline static void DeepCopy(const T& source, T& destination)
  {
    typedef typename UnderlyingType<T>::type tUnderlying;
    rtti::DeepCopyOperation<tUnderlying>::DeepCopy(reinterpret_cast<const tUnderlying&>(source), reinterpret_cast<tUnderlying&>(destination));
  }
};


template <typename T, bool Tis_default_construction_zero_memory = IsDefaultConstructionZeroMemory<T>::value, bool Tno_arg_constructor = std::is_base_of<serialization::DefaultImplementation, serialization::DefaultInstantiation<T>>::value>
struct ConstructorFunction
{
  static void Construct(void* placement)
  {
    new(placement) T();
  }

  static constexpr operations::tConstructor value = &Construct;
};

template <typename T>
struct ConstructorFunction<T, false, false>
{
  static void Construct(void* placement)
  {
    new(placement) T(serialization::DefaultInstantiation<T>::Create());
  }

  static constexpr operations::tConstructor value = &Construct;
};

template <typename T, bool Tno_arg_constructor>
struct ConstructorFunction<T, true, Tno_arg_constructor>
{
  void* const value = nullptr;
};


template <typename T, bool Tsupports_bitwise_copy = SupportsBitwiseCopy<T>::value>
struct DeepCopyFunction
{
  static void DeepCopy(const tTypedConstPointer& source, const tTypedPointer& destination)
  {
    rtti::DeepCopyOperation<T>::DeepCopy(*source.GetUnchecked<T>(), *destination.GetUnchecked<T>());
  }

  static constexpr operations::tDeepCopy value = &DeepCopy;
};

template <typename T>
struct DeepCopyFunction<T, true>
{
  void* const value = nullptr;
};


template <typename T>
struct DeserializeFromBinaryFunction
{
  static void Deserialize(serialization::tInputStream& stream, const tTypedPointer& destination)
  {
    stream >> *destination.GetUnchecked<T>();
  }

  static constexpr operations::tDeserializeFromBinary value = &Deserialize;
};

template <typename T>
struct DeserializeFromStringFunction
{
  static void Deserialize(serialization::tStringInputStream& stream, const tTypedPointer& destination)
  {
    stream >> *destination.GetUnchecked<T>();
  }

  static constexpr operations::tDeserializeFromString value = &Deserialize;
};

template <typename T>
struct DeserializeFromXMLFunction
{
  static void Deserialize(const rrlib::xml::tNode& node, const tTypedPointer& destination)
  {
    node >> *destination.GetUnchecked<T>();
  }

  static constexpr operations::tDeserializeFromXML value = &Deserialize;
};


template < typename T, bool Tno_op = std::is_trivially_destructible<T>::value || SupportsBitwiseCopy<T>::value >
struct DestructorFunction
{
  static void Destruct(void* object)
  {
    static_cast<T*>(object)->~T();
  }

  static constexpr operations::tDestructor value = &Destruct;
};

template <typename T>
struct DestructorFunction<T, true>
{
  void* const value = nullptr;
};


template < typename T,
         bool Tequals_operator = HasEqualToOperator<T>::value,
         bool Tbinary_serializable = serialization::IsBinarySerializable<T>::value,
         bool Tsupports_bitwise_copy = SupportsBitwiseCopy<T>::value >
struct EqualsFunction
{
  static bool Equals(const tTypedConstPointer& object1, const tTypedConstPointer& object2)
  {
    return object1.GetRawDataPointer() == object2.GetRawDataPointer();  // Objects that cannot be compared are only the same if they have the same address
  }

  static constexpr operations::tEquals value = &Equals;
};

template <typename T, bool Tequals_operator, bool Tbinary_serializable>
struct EqualsFunction<T, Tequals_operator, Tbinary_serializable, true>
{
  void* const value = nullptr;
};

template <typename T, bool Tbinary_serializable>
struct EqualsFunction<T, true, Tbinary_serializable, false>
{
  static bool Equals(const tTypedConstPointer& object1, const tTypedConstPointer& object2)
  {
    return *object1.GetUnchecked<T>() == *object2.GetUnchecked<T>();
  }

  static constexpr operations::tEquals value = &Equals;
};

template <typename T>
struct EqualsFunction<T, false, true, false>
{
  static bool Equals(const tTypedConstPointer& object1, const tTypedConstPointer& object2)
  {
    return serialization::SerializationEquals(*object1.GetUnchecked<T>(), *object2.GetUnchecked<T>());
  }

  static constexpr operations::tEquals value = &Equals;
};

template < typename TVector,
         typename TElement = typename serialization::IsSerializableContainer<TVector>::tValue,
         bool Tequals_operator = HasEqualToOperator<TElement>::value,
         bool Tsupports_bitwise_copy = SupportsBitwiseCopy<TElement>::value >
struct EqualsFunctionVector
{
  static bool Equals(const tTypedConstPointer& object1, const tTypedConstPointer& object2)
  {
    const TVector& v1 = *object1.GetUnchecked<TVector>();
    const TVector& v2 = *object2.GetUnchecked<TVector>();
    return v1.size() == v2.size() && std::equal(v1.begin(), v1.end(), v2.begin(), &EqualsFunction<TElement>::Equals);
    //return *object1.GetUnchecked<TVector>() == *object2.GetUnchecked<TVector>();
  }

  static constexpr operations::tEquals value = &Equals;
};

template <typename TVector, typename TElement, bool Tsupports_bitwise_copy>
struct EqualsFunctionVector<TVector, TElement, true, Tsupports_bitwise_copy>
{
  static bool Equals(const tTypedConstPointer& object1, const tTypedConstPointer& object2)
  {
    const TVector& v1 = *object1.GetUnchecked<TVector>();
    const TVector& v2 = *object2.GetUnchecked<TVector>();
    return v1 == v2;
  }

  static constexpr operations::tEquals value = &Equals;
};

template <typename TVector, typename TElement, bool Tequals_operator>
struct EqualsFunctionVector<TVector, TElement, Tequals_operator, true>
{
  static bool Equals(const tTypedConstPointer& object1, const tTypedConstPointer& object2)
  {
    const TVector& v1 = *object1.GetUnchecked<TVector>();
    const TVector& v2 = *object2.GetUnchecked<TVector>();
    return v1.size() == v2.size() && memcmp(&(v1[0]), &(v2[0]), sizeof(TElement) * v1.size()) == 0;
  }

  static constexpr operations::tEquals value = &Equals;
};


template <typename T>
struct GetVectorElementFunction
{
  static tTypedConstPointer GetVectorElement(const tTypedConstPointer& data, size_t index)
  {
    const T& v = *data.GetUnchecked<T>();
    return index < v.size() ? tTypedConstPointer(&v[index], tDataType<typename serialization::IsSerializableContainer<T>::tValue>()) : tTypedConstPointer();
  }

  static constexpr operations::tGetVectorElement value = &GetVectorElement;
};

template <>
struct GetVectorElementFunction<std::vector<bool>>
{
  static tTypedConstPointer GetVectorElement(const tTypedConstPointer& data, size_t index)
  {
    throw std::runtime_error("GetVectorElement operation is not supported for std::vector<bool>");
  }

  static constexpr operations::tGetVectorElement value = &GetVectorElement;
};

template <typename T>
struct GetVectorSizeFunction
{
  static size_t GetVectorSize(const tTypedConstPointer& data)
  {
    const T& v = *data.GetUnchecked<T>();
    return v.size();
  }

  static constexpr operations::tGetVectorSize value = &GetVectorSize;
};

template <typename T>
struct ResizeVectorFunction
{
  static void ResizeVector(const tTypedPointer& data, size_t new_size)
  {
    T& v = *data.GetUnchecked<T>();
    serialization::ContainerResize<typename serialization::IsSerializableContainer<T>::tValue>::Resize(v, new_size);
  }

  static constexpr operations::tResizeVector value = &ResizeVector;
};

template <typename T>
struct SerializeToBinaryFunction
{
  static void Serialize(serialization::tOutputStream& stream, const tTypedConstPointer& source)
  {
    stream << *source.GetUnchecked<T>();
  }

  static constexpr operations::tSerializeToBinary value = &Serialize;
};

template <typename T>
struct SerializeToStringFunction
{
  static void Serialize(serialization::tStringOutputStream& stream, const tTypedConstPointer& source)
  {
    stream << *source.GetUnchecked<T>();
  }

  static constexpr operations::tSerializeToString value = &Serialize;
};

template <typename T>
struct SerializeToXMLFunction
{
  static void Serialize(rrlib::xml::tNode& node, const tTypedConstPointer& source)
  {
    node << *source.GetUnchecked<T>();
  }

  static constexpr operations::tSerializeToXML value = &Serialize;
};

} // namespace detail

template <typename T>
struct ConstructorFunction : std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, ConstructorFunction<typename UnderlyingType<T>::type>, detail::ConstructorFunction<typename NormalizedType<T>::type >>::type
{};

template <typename T>
struct DeepCopyFunction : std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, DeepCopyFunction<typename UnderlyingType<T>::type>, detail::DeepCopyFunction<typename NormalizedType<T>::type >>::type
{};

template <typename T>
struct DeserializeFromBinaryFunction : std::conditional < (!std::is_same<typename UnderlyingType<T>::type, T>::value) && (!UnderlyingType<T>::cBINARY_SERIALIZATION_DIFFERS), DeserializeFromBinaryFunction<typename UnderlyingType<T>::type>, detail::DeserializeFromBinaryFunction<typename NormalizedType<T>::type >>::type
    {};

template <typename T>
struct DeserializeFromStringFunction : std::conditional < (!std::is_same<typename UnderlyingType<T>::type, T>::value) && (!UnderlyingType<T>::cOTHER_SERIALIZATION_DIFFERS), DeserializeFromStringFunction<typename UnderlyingType<T>::type>, detail::DeserializeFromStringFunction<typename NormalizedType<T>::type >>::type
    {};

template <typename T>
struct DeserializeFromXMLFunction : std::conditional < (!std::is_same<typename UnderlyingType<T>::type, T>::value) && (!UnderlyingType<T>::cOTHER_SERIALIZATION_DIFFERS), DeserializeFromXMLFunction<typename UnderlyingType<T>::type>, detail::DeserializeFromXMLFunction<typename NormalizedType<T>::type >>::type
    {};

template <typename T>
struct DestructorFunction : std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, DestructorFunction<typename UnderlyingType<T>::type>, detail::DestructorFunction<typename NormalizedType<T>::type >>::type
{};

template <typename T>
struct EqualsFunction : std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, EqualsFunction<typename UnderlyingType<T>::type>, typename std::conditional<serialization::IsSerializableContainer<T>::value, detail::EqualsFunctionVector<typename NormalizedType<T>::type>, detail::EqualsFunction<typename NormalizedType<T>::type>>::type >::type
{};

template <typename T>
struct GetVectorElementFunction : std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, GetVectorElementFunction<typename UnderlyingType<T>::type>, detail::GetVectorElementFunction<typename NormalizedType<T>::type >>::type
{};

template <typename T>
struct GetVectorSizeFunction : std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, GetVectorSizeFunction<typename UnderlyingType<T>::type>, detail::GetVectorSizeFunction<typename NormalizedType<T>::type >>::type
{};

template <typename T>
struct ResizeVectorFunction : std::conditional < !std::is_same<typename UnderlyingType<T>::type, T>::value, ResizeVectorFunction<typename UnderlyingType<T>::type>, detail::ResizeVectorFunction<typename NormalizedType<T>::type >>::type
{};

template <typename T>
struct SerializeToBinaryFunction : std::conditional < (!std::is_same<typename UnderlyingType<T>::type, T>::value) && (!UnderlyingType<T>::cBINARY_SERIALIZATION_DIFFERS), SerializeToBinaryFunction<typename UnderlyingType<T>::type>, detail::SerializeToBinaryFunction<typename NormalizedType<T>::type >>::type
    {};

template <typename T>
struct SerializeToStringFunction : std::conditional < (!std::is_same<typename UnderlyingType<T>::type, T>::value) && (!UnderlyingType<T>::cOTHER_SERIALIZATION_DIFFERS), SerializeToStringFunction<typename UnderlyingType<T>::type>, detail::SerializeToStringFunction<typename NormalizedType<T>::type >>::type
    {};

template <typename T>
struct SerializeToXMLFunction : std::conditional < (!std::is_same<typename UnderlyingType<T>::type, T>::value) && (!UnderlyingType<T>::cOTHER_SERIALIZATION_DIFFERS), SerializeToXMLFunction<typename UnderlyingType<T>::type>, detail::SerializeToXMLFunction<typename NormalizedType<T>::type >>::type
    {};

template <typename T>
struct DeepCopyOperation : detail::DeepCopyOperation<T>
{};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
