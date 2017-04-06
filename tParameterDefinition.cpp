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
/*!\file    rrlib/rtti/tParameterDefinition.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2017-02-22
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/tParameterDefinition.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

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
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tParameterDefinition::tParameterDefinition(const tParameterDefinition& other) :
  name(other.name.Get(), other.name.OwnsBuffer()),
  owned_default(other.owned_default ? other.owned_default->GetType().CreateGenericObject() : nullptr),
  type_and_default(owned_default ? owned_default->GetRawDataPointer() : other.type_and_default.GetRawDataPointer(), other.type_and_default.GetType()),
  is_static(other.is_static)
{
  if (owned_default)
  {
    owned_default->DeepCopyFrom(*other.owned_default);
  }
}

tParameterDefinition& tParameterDefinition::operator=(const tParameterDefinition& other)
{
  tParameterDefinition new_definition(other);
  std::swap(*this, new_definition);
  return *this;
}

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tParameterDefinition& definition)
{
  stream << definition.GetName() << definition.GetType() << definition.IsStatic();
  stream.WriteBoolean(definition.GetDefaultValue().GetRawDataPointer());
  if (definition.GetDefaultValue().GetRawDataPointer())
  {
    definition.GetDefaultValue().Serialize(stream);
  }
  return stream;
}

serialization::tInputStream& operator >> (serialization::tInputStream& stream, tParameterDefinition& definition)
{
  std::string name;
  rrlib::rtti::tType type;
  bool is_static = false;
  stream >> name >> type >> is_static;
  if (stream.ReadBoolean())
  {
    char memory[type.GetSize(true)];
    auto temp_default_buffer = type.EmplaceGenericObject(memory);
    temp_default_buffer->Deserialize(stream);
    tParameterDefinition new_definition(name.c_str(), *temp_default_buffer, is_static, true, true);
    std::swap(new_definition, definition);
  }
  else
  {
    tParameterDefinition new_definition(name.c_str(), type, true);
    std::swap(new_definition, definition);
  }
  return stream;
}

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tConstParameterDefinitionRange& definition_range)
{
  size_t size = definition_range.End() - definition_range.Begin();
  if (size < 0xFF)
  {
    stream.WriteByte(size);
  }
  else
  {
    stream.WriteByte(0xFF);
    stream.WriteLong(size);
  }
  for (auto it = definition_range.Begin(); it < definition_range.End(); ++it)
  {
    stream << (*it);
  }

  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
