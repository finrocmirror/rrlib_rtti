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
/*!\file    rrlib/rtti/detail/tDataType.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief   Contains tDataType
 *
 * \b tDataType
 *
 * Objects of this class contain and register runtime information about the
 * data type T.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__tDataType_h__
#define __rrlib__rtti__detail__tDataType_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <string>
#include <typeinfo>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"
#include "rrlib/rtti/type_traits.h"
#include "rrlib/rtti/tType.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
namespace serialization
{
class tInputStream;
class tOutputStream;
}
namespace rtti
{
class tGenericObject;

namespace detail
{

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Runtime type information for T
/*!
 * Objects of this class contain and register runtime information about the
 * data type T.
 */
template<typename T>
class tDataType : public tType
{
  template <typename U, bool Enum>
  class tDataTypeInfo;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tDataType() : tType(GetDataTypeInfo())
  {
  }

  /*!
   * Custom name may only be specified on first instantiation of a tDataType<T> for each type T
   *
   * \param name Name data type should get (if different from default)
   */
  tDataType(const std::string& name) : tType(GetDataTypeInfo(name.c_str()))
  {
  }

  /*!
   * Lookup data type by rtti name
   *
   * Tries T first
   * \param rtti_name rtti name
   * \return Data type with specified name (== NULL if it could not be found)
   */
  static tType FindTypeByRtti(const char* rtti_name)
  {
    if (rtti_name == GetDataTypeInfo()->rtti_name)
    {
      return tDataType();
    }
    return tType::FindTypeByRtti(rtti_name);
  }

  /*!
   * \param name Custom name for data type (may only be specified on first call/instantiation)
   * \return DataTypeInfo for this type T
   */
  inline static tInfo* GetDataTypeInfo(const char* name = nullptr)
  {
    static tDataTypeInfo<T, std::is_enum<T>::value> info(name);
    if (name && info.name != name) // hopefully, compiler optimizes this away for all calls with name == nullptr
    {
      RRLIB_LOG_PRINT_STATIC(ERROR, "Type name '", info.name, "' can only be changed on initial instantiation of tDataType<T>.");
    }
    assert(((!name) || info.name == name) && "Type name may not be changed later"); // assertion for efficiency reasons
    return &info;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*!
   * Data type info with factory functions
   */
  class tDataTypeInfoBase : public tType::tInfo
  {
  public:

    tDataTypeInfoBase(tType::tClassification classification, const std::string& name);

    virtual tGenericObject* CreateInstanceGeneric(void* placement, bool emplace_generic_object) const override;

    virtual void DeepCopy(const void* src, void* dest, tFactory* f) const override;

    virtual void Deserialize(serialization::tInputStream& is, void* obj) const override;

    virtual void Serialize(serialization::tOutputStream& os, const void* obj) const override;

    virtual void Init() override
    {
      AutoRegisterRelatedTypes<T>::Register();
    }
  };

  template <typename U, bool Enum>
  class tDataTypeInfo : public tDataTypeInfoBase
  {
  public:
    tDataTypeInfo(const char* name) : tDataTypeInfoBase(tClassification::PLAIN, name ? name : TypeName<T>::Get().c_str()) {}
  };

  template <typename U>
  class tDataTypeInfo<std::vector<U>, false> : public tDataTypeInfoBase
  {
  public:
    tDataTypeInfo(const char* name) : tDataTypeInfoBase(tClassification::LIST, name ? name : TypeName<T>::Get().c_str())
    {
      this->element_type = tDataType<U>::GetDataTypeInfo();
      this->element_type->list_type = this;
    }
  };

  template <typename U>
  class tDataTypeInfo<U, true> : public tDataTypeInfoBase
  {
  public:
    tDataTypeInfo(const char* name) : tDataTypeInfoBase(tClassification::PLAIN, name ? name : TypeName<T>::Get().c_str())
    {
      this->enum_strings = &make_builder::internal::GetEnumStrings<T>();
      if (this->enum_strings->non_standard_values)
      {
        for (size_t i = 0; i < this->enum_strings->size; i++)
        {
          this->non_standard_enum_value_strings.emplace_back(std::to_string(static_cast<typename std::underlying_type<T>::type>(static_cast<const T*>(this->enum_strings->non_standard_values)[i])));
        }
      }
    }
  };
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#include "rrlib/rtti/detail/tDataType.hpp"

#endif
