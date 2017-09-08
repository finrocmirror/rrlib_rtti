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
/*!\file    rrlib/rtti/detail/tTypeInfo.h
 *
 * \author  Max Reichardt
 *
 * \date    2016-07-20
 *
 * \brief   Contains tTypeInfo
 *
 * \b tTypeInfo
 *
 * Base class for all data type information handled by rrlib_rtti
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__tTypeInfo_h__
#define __rrlib__rtti__detail__tTypeInfo_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <vector>
#include <string>
#include "rrlib/serialization/tRegister.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/util/tManagedConstCharPointer.h"

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

class tType;

namespace detail
{

/*!
 * Functions for TypeName trait
 *
 * \param type Data type (everything but name is initialized)
 * \return Name for type (nullptr to keep default name)
 */
typedef util::tManagedConstCharPointer(*tGetTypenameFunction)(const tType& type);
typedef std::vector<util::tManagedConstCharPointer>(*tGetTypenamesFunction)(const tType& type);


//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Data type info
/*!
 * Base for all data type information handled by rrlib_rtti (should be compile time constant)
 */
struct tTypeInfo
{

  /*!
   * Shared data type info (possibly shared between plain types an their respective list types)
   */
  class tSharedInfo
  {
  public:

    /*! Data type of type register */
    typedef serialization::tRegister<tType, 32, 128> tRegisteredTypes;

    /*!
     * \param type_info Type info of plain type
     * \param underlying_type Type info of underlying type referenced in UnderlyingType trait
     * \param get_typename_function Function to obtain name of plain type with
     * \param name Name of plain type
     * \param list_type Unused parameter (used to auto-register list types)
     * \param auto_registered Unused parameter (used for additional auto-registration)
     * \param register_types_now Register type at the end of constructor? (should be false, for subclasses)
     */
    tSharedInfo(const tTypeInfo* type_info, tGetTypenameFunction get_typename_function, const tTypeInfo* list_type, int auto_registered, bool register_types_now = true);
    tSharedInfo(const tTypeInfo* type_info, tGetTypenamesFunction get_typename_function, const tTypeInfo* list_type, int auto_registered, bool register_types_now = true);
    tSharedInfo(const tTypeInfo* type_info, util::tManagedConstCharPointer name, const tTypeInfo* list_type, int auto_registered, bool register_types_now = true);
    tSharedInfo(const tTypeInfo* type_info, const char* name, const tTypeInfo* list_type, int auto_registered, bool register_types_now = true);

    /*!
     * Constructor for null-type
     */
    tSharedInfo(std::nullptr_t);

    /*!
     * Sets name
     * Note that replacing this name in template arguments of any already registered types in not done.
     *
     * \param new_name New name for element
     * \param type_info Type info to set name for
     */
    void SetName(util::tManagedConstCharPointer new_name, const tTypeInfo* type_info);

  protected:

    /*!
     * Registers type (type is assigned handle in this step and added to list of available types)
     *
     * \param type_info Type info of type to register
     */
    void Register(const tTypeInfo* type_info);

  private:

    friend class rtti::tType;
    friend struct tTypeInfo;


    /*! Name of plain data type */
    const char* name;

    /*! Data type handle */
    uint16_t handle;

    /*! Register of registered types */
    static const tRegisteredTypes* registered_types;


    tSharedInfo(const tTypeInfo* type_info, util::tManagedConstCharPointer name, bool non_standard_name, bool register_types_now);

    /*!
     * Adds name for lookup of specified data type (e.g. to support legacy data type names)
     *
     * \param type_info Data type that additional name is for
     * \param name Another name supported for lookup
     */
    static void AddName(const tTypeInfo* type_info, util::tManagedConstCharPointer name);

    /*!
     * \param type_name Type name (in rrlib::rtti) format
     * \param The same type name without any namespaces (e.g. returns 'Pose2D' for 'rrlib.math.Pose2D')
     */
    static std::string RemoveNamespaces(const std::string& type_name);
  };

  class tSharedInfoEnum : public tSharedInfo
  {
  public:

    /*!
     * \param type_info Type info of plain type
     * \param get_typename_function Function to obtain name of plain type with
     * \param enum_strings Enum String for this type
     * \param list_type Unused parameter (used to auto-register list types)
     * \param auto_registered Unused parameter (used for additional auto-registration)
     */
    tSharedInfoEnum(const tTypeInfo* type_info, tGetTypenameFunction get_typename_function, const make_builder::internal::tEnumStrings& enum_strings, const tTypeInfo* list_type, int auto_registered);

    /*! pointer to enum string constants data - if this is an enum type */
    const make_builder::internal::tEnumStrings& enum_strings;
  };

  /*! Type info for null/empty type */
  static const tTypeInfo cNULL_TYPE_INFO;

  /*! Std type info of type (typeid(T)) */
  const std::type_info& std_type_info;

  /*! Bit vector of type traits determined at compile time (see TypeTraitsVector) + from constructor */
  uint32_t type_traits;

  /*! Contains pointer to underlying type (see UnderlyingType type trait) */
  const tTypeInfo* underlying_type;

  /*! Contains pointer to element type (see ElementType type trait) */
  const tTypeInfo* element_type;

  /* Pointer to shared type info (initialized at runtime) */
  tSharedInfo* shared_info;

  /*! sizeof(T) - required by some generic functions */
  uint32_t size;


  constexpr tTypeInfo(const std::type_info& std_type_info, uint32_t type_traits, const tTypeInfo* underlying_type, const tTypeInfo* element_type, tSharedInfo* shared_info, uint32_t size) :
    std_type_info(std_type_info),
    type_traits(type_traits),
    underlying_type(underlying_type == nullptr ? this : underlying_type),
    element_type(element_type),
    shared_info(shared_info),
    size(size)
  {}


  enum { cLIST_TRAIT_FLAGS = 1 | (1 << 12) };   // the complete set of flags is declared in type_traits.h

  /*!
   * Lookup data type by name.
   *
   * If a match with the complete namespace cannot be found, then
   * 1) if 'name' does not contain a namespace, any type with the same name including a namespace will be returned as a match
   * 2) if 'name' contains a namespace, a type with the same name but without namespace will be returned as a match
   *
   * \param name Data Type name
   * \return Data type with specified name (== cNULL_TYPE_INFO if it could not be found)
   */
  static const tType FindType(const std::string& name);

  /*!
   * Lookup data type by rtti name
   *
   * \param rtti_name rtti name
   * \return Data type with specified name (== cNULL_TYPE_INFO if it could not be found)
   */
  static const tType FindTypeByRtti(const char* rtti_name);

  /*!
   * Computes type name in default rrlib::rtti format from rtti type name.
   * (no 't' prefixes; '.' instead of '::' for namespace separation; e.g. "rrlib.distance_data.DistanceData")
   *
   * \param type Type info (including mangled type name from typeid(...).name())
   * \return Name that rrlib_rtti would assign to this type by default
   */
  static util::tManagedConstCharPointer GetDefaultTypeName(const tType& type);

  /*!
   * \return Handle of data type
   */
  inline uint16_t GetHandle() const
  {
    return shared_info->handle;
  }

  /*!
   * For the types registered in rrlib::rtti, returns the assigned name
   *
   * \param type Type info (including mangled type name from typeid(...).name())
   * \return Name that rrlib_rtti assigns to this type
   */
  static util::tManagedConstCharPointer GetTypeNameDefinedInRRLibRtti(const tType& type);

  /*!
   * (note: this is significantly more efficient than calling FindType(name))
   *
   * \param name Name to check
   * \return Whether any of this type's names equals 'name'
   */
  bool HasName(const std::string& name) const;

  /*!
   * \return Is this a list type? (std::vector<T> of some type T)
   */
  inline bool IsListType() const
  {
    return (type_traits & cLIST_TRAIT_FLAGS) == cLIST_TRAIT_FLAGS;
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
