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
/*!\file    rrlib/rtti/tParameterDefinition.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-02-22
 *
 * \brief   Contains tParameterDefinition
 *
 * \b tParameterDefinition
 *
 * Generic parameter definition with name, data type, and possibly default value - fully serializable.
 * Such generic parameters are required in generic parts of some RRLibs and Finroc.
 * Related to generic objects, these parameter definitions fit reasonably well to rrlib_rtti - but could also be separated.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tParameterDefinition_h__
#define __rrlib__rtti__tParameterDefinition_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tIteratorRange.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tGenericObject.h"

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

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Generic parameter definition
/*!
 * Generic parameter definition with name, data type, and possibly default value - fully serializable.
 * Such generic parameters are required in generic parts of some RRLibs and Finroc.
 * Related to generic objects, these parameter definitions fit reasonably well to rrlib_rtti - but could also be separated.
 */
class tParameterDefinition
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tParameterDefinition() : is_static(false)
  {}

  tParameterDefinition(const tParameterDefinition& other);

  /*!
   * \param name Name of parameter
   * \param data_type Data type of parameter
   * \param is_static Whether parameter is static (or constant) and may not be changed at application runtime
   * \param copy_name Whether 'name' should be copied - or whether it can safely be referenced (can be false for all string literals)
   */
  tParameterDefinition(const char* name, const tType& data_type, bool is_static, bool copy_name = false) :
    name(name, copy_name),
    owned_default(),
    type_and_default(nullptr, data_type),
    is_static(is_static)
  {}

  /*!
   * \param name Name of parameter
   * \param default_value Default value of parameter
   * \param is_static Whether parameter is static (or constant) and may not be changed at application runtime
   * \param copy_name Whether 'name' should be copied - or whether it can safely be referenced (can be false for all string literals)
   * \param copy_default_value Whether 'default_value' should be copied - or whether it can safely be referenced
   */
  tParameterDefinition(const char* name, const tTypedConstPointer& default_value, bool is_static, bool copy_name = false, bool copy_default_value = false) :
    name(name, copy_name),
    owned_default(copy_default_value ? default_value.GetType().CreateGenericObject() : nullptr),
    type_and_default(copy_default_value ? owned_default->GetRawDataPointer() : default_value.GetRawDataPointer(), default_value.GetType()),
    is_static(is_static)
  {
    if (owned_default)
    {
      owned_default->DeepCopyFrom(default_value);
    }
  }

  /*!
   * \param name Name of parameter
   * \param default_value Default value of parameter (pointer to avoid ambiguities with other constructors)
   * \param is_static Whether parameter is static (or constant) and may not be changed at application runtime
   * \param copy_name Whether 'name' should be copied - or whether it can safely be referenced (can be false for all string literals)
   * \param copy_default_value Whether 'default_value' should be copied - or whether it can safely be referenced
   */
  template <typename T>
  tParameterDefinition(const char* name, const T* default_value, bool is_static, bool copy_name = false, bool copy_default_value = false) :
    tParameterDefinition(name, tTypedConstPointer(default_value), copy_name, copy_default_value)
  {
  }

  tParameterDefinition& operator=(const tParameterDefinition& other);

  /*!
   * \return Default value of parameter. May contain nullptr if no default value has been set.
   */
  const tTypedConstPointer& GetDefaultValue() const
  {
    return type_and_default;
  }

  /*!
   * \return Name of parameter
   */
  const char* GetName() const
  {
    return name.Get();
  }

  /*!
   * \return Data type of parameter
   */
  const tType& GetType() const
  {
    return type_and_default.GetType();
  }

  /*!
   * \return False if this is an empty object with no parameter definition (e.g. created by default constructor)
   */
  bool IsDefined() const
  {
    return name.Get() != nullptr;
  }

  /*!
   * \return Whether parameter is static (or constant) and may not be changed at application runtime
   */
  bool IsStatic() const
  {
    return is_static;
  }

  operator bool() const
  {
    return IsDefined();
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Name of parameter */
  rrlib::util::tManagedConstCharPointer name;

  /*! If default value was copied: Contains copy */
  std::unique_ptr<tGenericObject> owned_default;

  /*! Type and default value of parameter (nullptr if no default value is set) */
  tTypedConstPointer type_and_default;

  /*! Whether parameter is static (or constant) and may not be changed at application runtime */
  bool is_static;
};

typedef rrlib::util::tIteratorRange<tParameterDefinition*> tParameterDefinitionRange;
typedef rrlib::util::tIteratorRange<const tParameterDefinition*> tConstParameterDefinitionRange;

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tParameterDefinition& definition);
serialization::tInputStream& operator >> (serialization::tInputStream& stream, tParameterDefinition& definition);
serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tConstParameterDefinitionRange& definition_range);
inline serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tParameterDefinitionRange& definition_range)
{
  stream << tConstParameterDefinitionRange(definition_range.Begin(), definition_range.End());
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
