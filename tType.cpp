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
/*!\file    rrlib/rtti/tType.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-11-02
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/tType.h"

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

namespace
{

template <typename T>
inline void StreamChars(std::ostream& stream, const T chars)
{
  stream << chars;
}

template <size_t Tchars>
inline void StreamChars(serialization::tOutputStream& stream, const char(&chars)[Tchars])
{
  stream.Write(chars, Tchars - 1);
}
inline void StreamChars(serialization::tOutputStream& stream, const char* chars)
{
  stream.Write(chars, strlen(chars));
}
inline void StreamChars(serialization::tOutputStream& stream, char c)
{
  stream.WriteByte(c);
}

template <typename TStream>
void StreamType(TStream& stream, const tType& type)
{
  switch (type.GetTypeClassification())
  {
  case tTypeClassification::RPC_TYPE:
  case tTypeClassification::OTHER_DATA_TYPE:
  case tTypeClassification::INTEGRAL:
  case tTypeClassification::NULL_TYPE:
    StreamChars(stream, type.GetPlainTypeName());
    break;
  case tTypeClassification::ARRAY:
    StreamChars(stream, "Array<");
    StreamType(stream, type.GetElementType());
    StreamChars(stream, ", ");
    char buffer[100];
    sprintf(buffer, "%d", type.GetArraySize());
    StreamChars(stream, buffer);
    StreamChars(stream, '>');
    break;
  case tTypeClassification::LIST:
    StreamChars(stream, "List<");
    StreamType(stream, type.GetElementType());
    StreamChars(stream, '>');
    break;
  case tTypeClassification::ENUM_BASED_FLAGS:
    StreamChars(stream, "EnumFlags<");
    StreamType(stream, type.GetElementType());
    StreamChars(stream, '>');
    break;
  case tTypeClassification::PAIR:
  {
    std::pair<const detail::tTypeInfo::tTupleElementInfo*, size_t> tuple_types = type.GetTupleTypes();
    assert(tuple_types.second == 2);
    StreamChars(stream, "Pair<");
    StreamType(stream, tType(tuple_types.first[0].type_info));
    StreamChars(stream, ", ");
    StreamType(stream, tType(tuple_types.first[1].type_info));
    StreamChars(stream, '>');
    break;
  }
  case tTypeClassification::TUPLE:
  {
    std::pair<const detail::tTypeInfo::tTupleElementInfo*, size_t> tuple_types = type.GetTupleTypes();
    StreamChars(stream, "Tuple<");
    for (size_t i = 0; i < tuple_types.second; i++)
    {
      StreamType(stream, tType(tuple_types.first[i].type_info));
      StreamChars(stream, i == tuple_types.second - 1 ? ">" : ", ");
    }
    break;
  }
  default:
    break;
  }
}

}

tType tType::GetListType() const
{
  for (const tType & type : GetTypeRegister())
  {
    if (type.IsListType() && type.GetElementType() == *this)
    {
      return tType(type.info);
    }
  }
  return tType();
}

std::string tType::GetName() const
{
  if (GetTypeClassification() <= tTypeClassification::AUTO_NAMED)
  {
    std::stringstream stream;
    StreamType(stream, *this);
    return stream.str();
  }
  else
  {
    return GetSharedInfo().name;
  }
}

std::ostream& operator << (std::ostream& stream, const tType& type)
{
  StreamType(stream, type);
  return stream;
}

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tType& type)
{
  if (tType::GetTypeRegister().WriteEntry(stream, type.GetHandle()))
  {
    StreamType(stream, type);
    stream.WriteByte(0);
  }
  return stream;
}

serialization::tInputStream& operator >> (serialization::tInputStream& stream, tType& type)
{
  if (tType::GetTypeRegister().ReadEntry(stream, type))
  {
    type = tType::FindType(stream.ReadString());
  }
  return stream;
}

serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tType& type)
{
  StreamType(stream.GetWrappedStringStream(), type);
  return stream;
}

serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tType& type)
{
  std::string s;
  stream >> s;
  type = tType::FindType(s);
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
