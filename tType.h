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
/*!\file    rrlib/rtti/tType.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-11-02
 *
 * \brief   Contains tType
 *
 * \b tType
 *
 * Class with runtime information on all kinds of C++ types.
 *
 * A unique 16 bit type-id is assigned to each type.
 *
 * Can be used as factory to create instances of C++ classes
 * (necessary for deserializing vectors containing pointers).
 *
 * To create a tType object containing information on a C++ class,
 * an instance of DataType<T> is to be created.
 *
 * One such object needs be created before a type T can be
 * deserialized/looked up etc. generically.
 *
 * tType is passed by value
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tType_h__
#define __rrlib__rtti__tType_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tNoncopyable.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/detail/tTypeInfo.h"
#include "rrlib/rtti/generic_operations.h"
#include "rrlib/rtti/type_traits.h"

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

class tGenericObject;
struct tGenericObjectDestructorCall;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Data type
/*!
 * Class with runtime information on all kinds of C++ types.
 *
 * A unique 16 bit type-id is assigned to each type.
 *
 * Can be used as factory to create instances of C++ classes
 * (necessary for deserializing vectors containing pointers).
 *
 * To create a tType object containing information on a C++ class,
 * an instance of DataType<T> is to be created.
 *
 * One such object needs be created before a type T can be
 * deserialized/looked up etc. generically.
 *
 * tType is passed by value
 */
class tType
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  constexpr tType() : info(&detail::tTypeInfo::cNULL_TYPE_INFO) {}
  explicit constexpr tType(const detail::tTypeInfo* info) : info(info)
  {}


  /*!
   * Adds name for lookup of this data type (e.g. to support legacy data type names)
   *
   * \param name Another name supported for lookup
   */
  inline tType& AddName(const char* name)
  {
    detail::tTypeInfo::tSharedInfo::AddName(info, util::tManagedConstCharPointer(name, false));
    return *this;
  }


  inline tGenericObject* CreateInstanceGeneric() const __attribute__((deprecated))
  {
    return CreateGenericObject();
  }

  /*!
   * Creates object of this data type wrapped as tGenericObject
   *
   * \return Instance of data type wrapped as tGenericObject (caller is responsible for deleting)
   */
  inline tGenericObject* CreateGenericObject() const;

  /*!
   * Creates object of this data type wrapped as tGenericObject
   *
   * \param wrapped_data_placement Places wrapped data of this type (without tGenericObject object) at this memory address (needs to have at least size GetSize(false))
   * \return Instance of data type wrapped as tGenericObject (caller is responsible for deleting)
   */
  inline tGenericObject* CreateGenericObject(void* wrapped_data_placement) const;

  /*!
   * Destruct object instance at specified memory address.
   *
   * \param address Address where previously constructed object is located (if nullptr, nothing is done)
   */
  inline void DestructInstance(void* address) const
  {
    if ((info->type_traits & (trait_flags::cSUPPORTS_BITWISE_COPY | trait_flags::cHAS_TRIVIAL_DESTRUCTOR | trait_flags::cTYPE_CLASSIFICATION_BITS)) < static_cast<uint>(tTypeClassification::RPC_TYPE) && address)
    {
      (*GetBinaryOperations().destructor)(address);
    }
  }

  /*!
   * Default-construct object at specified memory address.
   * Caller must ensure that
   * (1) memory is valid and large enough
   * (2) objects with non-trivial destructor are properly destructed using DestructInstance function above
   *
   * \param placement Address to create object at (destination for placement new)
   */
  inline void EmplaceInstance(void* placement) const
  {
    if ((info->type_traits & (trait_flags::cIS_DEFAULT_CONSTRUCTION_ZERO_MEMORY | trait_flags::cTYPE_CLASSIFICATION_BITS)) < static_cast<uint>(tTypeClassification::RPC_TYPE))
    {
      (*GetBinaryOperations().constructor)(placement);
    }
    else
    {
      memset(placement, 0, info->size);
    }
  }

  /*!
   * Creates object of this data type wrapped as tGenericObject at specified memory address
   *
   * \param placement Destination for placement new (needs to have at least size GetSize(true))
   * \return Instance of data type wrapped as tGenericObject. Smart Pointer ensures that destructor on emplaced instance is called (-> smart pointer must not be destructed as long as object is in use)
   */
  inline std::unique_ptr<tGenericObject, tGenericObjectDestructorCall> EmplaceGenericObject(void* placement) const;

  /*!
   * Lookup data type by name.
   *
   * If a match with the complete namespace cannot be found, then
   * 1) if 'name' does not contain a namespace, any type with the same name including a namespace will be returned as a match
   * 2) if 'name' contains a namespace, a type with the same name but without namespace will be returned as a match
   *
   * \param name Data Type name
   * \return Data type with specified name (== nullptr if it could not be found)
   */
  static inline tType FindType(const std::string& name)
  {
    return detail::tTypeInfo::FindType(name);
  }

  /*!
   * Lookup data type by rtti name
   *
   * \param rtti_name rtti name
   * \return Data type with specified name (== nullptr if it could not be found)
   */
  static inline tType FindTypeByRtti(const char* rtti_name)
  {
    return detail::tTypeInfo::FindTypeByRtti(rtti_name);
  }

  /*!
   * \return Size of array if this is an array type; otherwise 1
   */
  inline size_t GetArraySize() const
  {
    return IsArray() ? (GetSize() / GetElementType().GetSize()) : 1;
  }

  /*!
   * \return Type returned by ElementType type trait
   */
  inline tType GetElementType() const
  {
    return tType(info->element_type);
  }

  /*!
   * \return Pointer to enum strings data if this is an enum type - otherwise nullptr
   */
  inline const make_builder::internal::tEnumStrings* GetEnumStringsData() const
  {
    return info->type_traits & trait_flags::cIS_ENUM ? &static_cast<detail::tTypeInfo::tSharedInfoEnum&>(GetSharedInfo()).enum_strings : nullptr;
  }

  /*!
   * \return Handle of data type
   */
  inline uint16_t GetHandle() const
  {
    return GetSharedInfo().handle;
  }

  /*!
   * \return If this is a plain type and a list type has been initialized: list type (std::vector<T>) - otherwise NULL
   */
  tType GetListType() const;

  /*!
   * (this is not particularly efficient as it allocates memory - use alternatives such as GetName(const char*, size_t) or stream operators if efficiency/real-time is desired)
   *
   * \return Name of data type
   */
  std::string GetName() const;

  /*!
   * \return Returns type name of plain/element type (this is == GetName() for all types whose names are not auto-generated)
   */
  const char* GetPlainTypeName() const
  {
    return GetTypeClassification() <= tTypeClassification::AUTO_NAMED ? GetElementType().GetPlainTypeName() : GetSharedInfo().name;
  }

  /*!
   * \return rtti name of data type  (note that this is the 'normalized' type for integral types - e.g. "i" for 'long' on 32 bit platforms)
   */
  inline const char* GetRttiName() const
  {
    return info->std_type_info.name();
  }

  /*!
   * \param Obtain size as generic object?
   * \return size of data type (as returned from sizeof(T) or sizeof(tGenericObject) + sizeof(T))
   */
  inline size_t GetSize(bool as_generic_object = false) const;

  /*!
   * If this type is a std::pair or std::tuple, returns elements types (first points to first element of element type array, second is number of elements in this array)
   */
  std::pair<const detail::tTypeInfo::tTupleElementInfo*, size_t> GetTupleTypes() const
  {
    tTypeClassification classification = GetTypeClassification();
    typedef std::pair<const detail::tTypeInfo::tTupleElementInfo*, size_t> tResult;
    if (classification == tTypeClassification::PAIR || classification == tTypeClassification::TUPLE)
    {
      const detail::tTypeInfo::tSharedInfoTuple& tuple_info = static_cast<const detail::tTypeInfo::tSharedInfoTuple&>(GetSharedInfo());
      return tResult(tuple_info.elements, tuple_info.tuple_size);
    }
    return tResult(nullptr, 0);
  }

  /*!
   * \param handle Data type handle
   * \return Data type with specified handle (== nullptr if there's no type with this handle)
   */
  static tType GetType(size_t handle)
  {
    return (handle < detail::tTypeInfo::tSharedInfo::registered_types->Size()) ? tType((*detail::tTypeInfo::tSharedInfo::registered_types)[handle]) : tType();
  }

  /*!
   * \return Classication of this type
   */
  tTypeClassification GetTypeClassification() const
  {
    return static_cast<tTypeClassification>(info->type_traits & trait_flags::cTYPE_CLASSIFICATION_BITS);
  }

  /*!
   * \return Number of registered types
   */
  static size_t GetTypeCount()
  {
    return detail::tTypeInfo::tSharedInfo::registered_types->Size();
  }

  /*!
   * \return Global register with registered types
   */
  static const detail::tTypeInfo::tSharedInfo::tRegisteredTypes& GetTypeRegister()
  {
    return (*detail::tTypeInfo::tSharedInfo::registered_types);
  }

  /*!
   * \return Bit vector of type traits determined at compile time (see type_traits.h)
   */
  inline uint32_t GetTypeTraits() const
  {
    return info->type_traits;
  }

  /*!
   * \return Underlying type (see UnderlyingType type trait)
   */
  inline tType GetUnderlyingType() const
  {
    return tType(info->underlying_type);
  }

  /*!
   * (note: this is significantly more efficient than calling FindType(name))
   *
   * \param name Name to check
   * \return Whether any of this type's names equals 'name'
   */
  inline bool HasName(const std::string& name) const
  {
    return info->HasName(detail::tStringRange(name.c_str(), name.c_str() + name.length()));
  }
  inline bool HasName(const char* name) const
  {
    return info->HasName(detail::tStringRange(name, name + strlen(name)));
  }

  /*!
   * \return Is this an array type? (std::vector<T> of some type T)
   */
  inline bool IsArray() const
  {
    return (info->type_traits & trait_flags::cTYPE_CLASSIFICATION_BITS) == static_cast<uint>(tTypeClassification::ARRAY);
  }

  /*!
   * \return Is this a list type? (std::vector<T> of some type T)
   */
  inline bool IsListType() const
  {
    return (info->type_traits & trait_flags::cTYPE_CLASSIFICATION_BITS) == static_cast<uint>(tTypeClassification::LIST);
  }

  /*!
   * Convenience function to register data type with specified type and name
   *
   * \tparam T Data type
   * \param name Name of type (optional)
   */
  template <typename T>
  inline static tType& Register()
  {
    return tDataType<T>();
  }
  template <typename T>
  inline static tType& Register(const char* name = nullptr)
  {
    return tDataType<T>(name);
  }

  /*!
   * \return Reference to shared type info object
   */
  const detail::tTypeInfo::tSharedInfo& SharedTypeInfo() const
  {
    return *info->shared_info;
  }

  /*!
   * for checks against nullptr (if (type == nullptr) {...} )
   */
  /*bool operator== (std::nullptr_t* ptr) const
  {
    return info == &detail::tTypeInfo::cNULL_TYPE_INFO;
  }*/

  bool operator== (const tType& other) const
  {
    return info == other.info;
  }

  /*bool operator!= (std::nullptr_t* ptr) const
  {
    return info != &detail::tTypeInfo::cNULL_TYPE_INFO;
  }*/

  bool operator!= (const tType& other) const
  {
    return info != other.info;
  }

  bool operator< (const tType& other) const
  {
    return info < other.info;
  }

  /*!
   * \return True if type is not null-type
   */
  operator bool() const
  {
    return info != &detail::tTypeInfo::cNULL_TYPE_INFO;
  }

//----------------------------------------------------------------------
// Protected information class
//----------------------------------------------------------------------
protected:

  friend class tTypedConstPointer;
  friend class tTypedPointer;
  friend struct detail::tTypeInfo;

  /*!
   * \return Reference to shared info
   */
  detail::tTypeInfo::tSharedInfo& GetSharedInfo() const
  {
    return *info->shared_info;
  }

  /*!
   * \return Binary operations of this object (a varying number of them exists - depending on flags)
   */
  const tBinaryOperations& GetBinaryOperations() const
  {
    return *reinterpret_cast<const tBinaryOperations*>(info + 1);
  }

  /*!
   * \return Binary operations of this object (a varying number of them exists - depending on flags)
   */
  const tBinaryOperationsVector& GetBinaryOperationsVector() const
  {
    return *reinterpret_cast<const tBinaryOperationsVector*>(reinterpret_cast<const char*>(info) + sizeof(detail::tTypeInfo) + sizeof(tBinaryOperations));
  }

  /*!
   * \return Binary operation functions (existence depends on flags)
   */
  const tBinarySerializationOperations& GetBinarySerialization() const
  {
    return *reinterpret_cast<const tBinarySerializationOperations*>(reinterpret_cast<const char*>(info) + (info->type_traits & trait_flags::cSERIALIZATION_FUNCTION_OFFSET_BITS));
  }

  /*!
   * \return Binary operation functions (existence depends on flags)
   */
  const tStringSerializationOperations& GetStringSerialization() const
  {
    return *reinterpret_cast<const tStringSerializationOperations*>(reinterpret_cast<const char*>(info) + (info->type_traits & trait_flags::cSERIALIZATION_FUNCTION_OFFSET_BITS) + ((info->type_traits & trait_flags::cIS_BINARY_SERIALIZABLE) ? sizeof(tBinarySerializationOperations) : 0));
  }

  /*!
   * \return Binary operation functions (existence depends on flags)
   */
  const tXMLSerializationOperations& GetXMLSerialization() const
  {
    return *reinterpret_cast<const tXMLSerializationOperations*>(reinterpret_cast<const char*>(info) + (info->type_traits & trait_flags::cSERIALIZATION_FUNCTION_OFFSET_BITS) + ((info->type_traits & trait_flags::cIS_BINARY_SERIALIZABLE) ? sizeof(tBinarySerializationOperations) : 0));
  }



//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Pointer to data type info (should not be copied every time for efficiency reasons) */
  const detail::tTypeInfo* info;
};

std::ostream& operator << (std::ostream& stream, const tType& type);
serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tType& type);
serialization::tInputStream& operator >> (serialization::tInputStream& stream, tType& type);
serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tType& dt);
serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tType& dt);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}

#include "rrlib/rtti/tTypedPointer.h"

#endif
