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

  /*!
   * Add annotation to this data type.
   * Annotations added to the null/empty-type are discarded.
   * Adding annotations is not supported during static variable initialization phase (they might be zeroed out afterwards).
   *
   * \param annotation Type of annotation to add (is copied)
   *                   Requirements for suitable data types for annotations are
   *                   - SupportsBitwiseCopy<T>::value is true
   *                   - IsDefaultConstructionZeroMemory<T>::value
   *                   - sizeof(T) <= 2 * sizeof(void*)
   * \tparam T Annotation type.
   *           T is used for lookup later.
   *           Only one annotation object per type T may be added. A second call will overwrite the first value.
   */
  template <typename T>
  inline void AddAnnotation(const T& annotation)
  {
    static_assert(sizeof(annotation) <= 2 * sizeof(void*) && SupportsBitwiseCopy<T>::value && IsDefaultConstructionZeroMemory<T>::value, "Annotation type is unsuitable");
    if (info != &detail::tTypeInfo::cNULL_TYPE_INFO)
    {
      GetSharedInfo().AddAnnotation(annotation);
    }
  }

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
    if ((info->type_traits & (trait_flags::cSUPPORTS_BITWISE_COPY | trait_flags::cHAS_TRIVIAL_DESTRUCTOR | trait_flags::cIS_DATA_TYPE)) == trait_flags::cIS_DATA_TYPE && address)
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
    if ((info->type_traits & (trait_flags::cIS_DEFAULT_CONSTRUCTION_ZERO_MEMORY | trait_flags::cIS_DATA_TYPE)) == trait_flags::cIS_DATA_TYPE)
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
   * Get annotation of specified class
   *
   * \tparam T Class of annotation to obtain
   * \return Annotation. Zero-Default-constructed object if annotation has not been set for this type.
   */
  template <typename T>
  inline T GetAnnotation() const
  {
    if (detail::tTypeInfo::tSharedInfo::tAnnotationIndexHolder<T>::index.second)
    {
      T t;
      memcpy(&t, &GetSharedInfo().annotations[detail::tTypeInfo::tSharedInfo::tAnnotationIndexHolder<T>::index.first], sizeof(T));
      return t;
    }
    return T();
  }

  /*!
   * \return In case of list: type of elements; otherwise null-type
   */
  inline tType GetElementType() const
  {
    return IsListType() ? GetType(GetSharedInfo().handle[0]) : tType();
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
    return GetSharedInfo().handle[info->IsListType() ? 1 : 0];
  }

  /*!
   * \return If this is a plain type and a list type has been initialized: list type (std::vector<T>) - otherwise NULL
   */
  inline tType GetListType() const
  {
    return (info->type_traits & (trait_flags::cHAS_LIST_TYPE | trait_flags::cIS_DATA_TYPE)) == (trait_flags::cHAS_LIST_TYPE | trait_flags::cIS_DATA_TYPE) ? GetType(GetSharedInfo().handle[1]) : tType();
  }

  /*!
   * (this is not particularly efficient as it allocates memory - use alternatives such as GetPlainTypeName() or stream operators if efficiency/real-time is desired)
   *
   * \return Name of data type
   */
  std::string GetName() const
  {
    if (IsListType())
    {
      size_t length = strlen(GetSharedInfo().name);
      char buffer[length + 10];
      memcpy(buffer, "List<", 5);
      memcpy(buffer + 5, GetSharedInfo().name, length);
      buffer[5 + length] = '>';
      buffer[6 + length] = 0;
      return buffer;
    }
    return GetSharedInfo().name;
  }

  /*!
   * Efficient variant of GetName()
   *
   * \param buffer Buffer that name is written to. Should have size >= strlen(GetPlainTypeName() + 7)
   */
  void GetName(char* buffer) const
  {
    size_t length = strlen(GetSharedInfo().name);
    if (IsListType())
    {
      memcpy(buffer, "List<", 5);
      memcpy(buffer + 5, GetSharedInfo().name, length);
      buffer[5 + length] = '>';
      buffer[6 + length] = 0;
    }
    else
    {
      memcpy(buffer, GetSharedInfo().name, length + 1);
    }
  }

  /*!
   * \return Returns type name of plain/element type (this is == GetName() for all non-list types T; for std::vector<T> it is, however, also the one for T)
   */
  const char* GetPlainTypeName() const
  {
    return GetSharedInfo().name;
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
   * \param handle Data type handle
   * \return Data type with specified handle (== nullptr if there's no type with this handle)
   */
  static tType GetType(size_t handle)
  {
    return (handle < detail::tTypeInfo::tSharedInfo::registered_types->Size()) ? tType((*detail::tTypeInfo::tSharedInfo::registered_types)[handle]) : tType();
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
    tType underlying_plain_type(GetSharedInfo().underlying_type);
    return IsListType() ? underlying_plain_type.GetListType() : underlying_plain_type;
  }

  /*!
   * (note: this is significantly more efficient than calling FindType(name))
   *
   * \param name Name to check
   * \return Whether any of this type's names equals 'name'
   */
  inline bool HasName(const std::string& name) const
  {
    return info->HasName(name);
  }

  /*!
   * \return Is this a list type? (std::vector<T> of some type T)
   */
  inline bool IsListType() const
  {
    return info->IsListType();
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

  explicit constexpr tType(const detail::tTypeInfo* info) : info(info)
  {}

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
