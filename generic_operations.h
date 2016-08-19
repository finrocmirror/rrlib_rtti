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
/*!\file    rrlib/rtti/generic_operations.h
 *
 * \author  Max Reichardt
 *
 * \date    2016-07-21
 *
 * Defines generic operations supported by rrlib_rtti
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__generic_operations_h__
#define __rrlib__rtti__generic_operations_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/serialization/serialization.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
namespace xml
{
class tNode;
}
namespace rtti
{
class tTypedPointer;
class tTypedConstPointer;

/*!
 * These type traits define various generic operations for objects of a type T.
 * tTypedPointer provides all these operations at runtime (with only runtime type information available)
 *
 * The traits may be specialized in order to use better (e.g. faster, accurate, shared) operations for T.
 * Defaults are defined in the detail namespace.
 */
template <typename T>
struct ConstructorFunction;

template <typename T>
struct DeepCopyFunction;

template <typename T>
struct DeserializeFromBinaryFunction;

template <typename T>
struct DeserializeFromStringFunction;

template <typename T>
struct DeserializeFromXMLFunction;

template <typename T>
struct DestructorFunction;

template <typename T>
struct EqualsFunction;

template <typename T>
struct GetVectorElementFunction;

template <typename T>
struct GetVectorSizeFunction;

template <typename T>
struct ResizeVectorFunction;

template <typename T>
struct SerializeToBinaryFunction;

template <typename T>
struct SerializeToStringFunction;

template <typename T>
struct SerializeToXMLFunction;

/*!
 * Suitable DeepCopy operation when type T is known at runtime
 */
template <typename T>
struct DeepCopyOperation;


/*! (legacy) type trait */
template <typename T>
using GenericOperations = DeepCopyOperation<T>;


namespace operations
{

/*!
 * Function pointer for default-constructing object at specified memory address.
 * Caller must ensure that
 * (1) memory is valid and large enough
 * (2) objects with non-trivial destructor are properly destructed using destructor function below
 *
 * \param placement Address to create object at (destination for placement new)
 */
typedef void (*tConstructor)(void* placement);

/*!
 * Create a deep copy of an object.
 * A deep copy means that the destination object must not change if the source object is modified or deleted.
 * Serialization of source and destination objects are equal after calling this.
 * Caller must ensure that source and destination have the same types.
 *
 * \param source Pointer to source object to be copied
 * \param destination Pointer to destination object
 * \throws May throw std::exception on invalid arguments or other failure
 */
typedef void (*tDeepCopy)(const tTypedConstPointer& source, const tTypedPointer& destination);

/*!
 * Deserialize data from input stream
 *
 * \param stream Input stream
 * \param destination Object to fill with deserialized data
 * \throws Throws std::exception on invalid data in stream
 */
typedef void (*tDeserializeFromBinary)(serialization::tInputStream& stream, const tTypedPointer& destination);

/*!
 * Deserialize data from input stream
 *
 * \param stream Input stream
 * \param destination Object to fill with deserialized data
 * \throws Throws std::exception on invalid data in stream
 */
typedef void (*tDeserializeFromString)(serialization::tStringInputStream& stream, const tTypedPointer& destination);

/*!
 * Deserialize data from XML node
 *
 * \param node XML node
 * \param destination Object to fill with deserialized data
 * \throws Throws std::exception on invalid data in XML node
 */
typedef void (*tDeserializeFromXML)(const xml::tNode& node, const tTypedPointer& destination);

/*!
 * Function pointer to destructor
 *
 * \param object Pointer to object to destruct
 */
typedef void (*tDestructor)(void* object);

/*!
 * Returns whether data of two pointers is equal.
 * Serializing equal objects must produce identical data.
 *
 * \param object1 Data to compare
 * \param object2 Data to compare
 */
typedef bool (*tEquals)(const tTypedConstPointer& object1, const tTypedConstPointer& object2);

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
typedef tTypedConstPointer(*tGetVectorElement)(const tTypedConstPointer& data, size_t index);

/*!
 * \param data Data pointer
 * \return
 * - If data points to std::vector, returns std::vector::size()
 * - If data is nullptr, returns 0
 * - Otherwise, returns 1
 */
typedef size_t (*tGetVectorSize)(const tTypedConstPointer& data);

/*!
 * Resizes std::vector.
 *
 * \param data Pointer to std::vector
 * \param new_size New size
 * \throws Throws std::invalid_argument 'data' does not point std::vector
 */
typedef void (*tResizeVector)(const tTypedPointer& data, size_t new_size);

/*!
 * Serialize data to output stream
 *
 * \param stream Output stream
 * \param source Data to serialize to stream
 */
typedef void (*tSerializeToBinary)(serialization::tOutputStream& stream, const tTypedConstPointer& source);

/*!
 * Serialize data to output stream
 *
 * \param stream Output stream
 * \param source Data to serialize to stream
 */
typedef void (*tSerializeToString)(serialization::tStringOutputStream& stream, const tTypedConstPointer& source);

/*!
 * Serialize data to XML node
 *
 * \param node XML node
 * \param source Data to serialize to XML node
 */
typedef void (*tSerializeToXML)(xml::tNode& node, const tTypedConstPointer& source);

}

/*! A set of supported binary operations as they lie in function tables */
struct tBinaryOperations
{
  operations::tConstructor constructor;
  operations::tDestructor destructor;
  operations::tDeepCopy deep_copy;
  operations::tEquals equals;
};

struct tBinaryOperationsVector
{
  operations::tGetVectorElement get_vector_element;
  operations::tGetVectorSize get_vector_size;
  operations::tResizeVector resize_vector;
};

struct tBinarySerializationOperations
{
  operations::tDeserializeFromBinary deserialize;
  operations::tSerializeToBinary serialize;
};

struct tStringSerializationOperations
{
  operations::tDeserializeFromString deserialize;
  operations::tSerializeToString serialize;
};

struct tXMLSerializationOperations
{
  operations::tDeserializeFromXML deserialize;
  operations::tSerializeToXML serialize;
};

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}

#endif
