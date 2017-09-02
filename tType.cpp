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

tType tType::GetListType() const
{
  for (const tType & type : GetTypeRegister())
  {
    if ((type.GetTypeTraits() & trait_flags::cIS_LIST_TYPE) && type.GetElementType() == *this)
    {
      return tType(type.info);
    }
  }
  return tType();
}

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tType& type)
{
  if (tType::GetTypeRegister().WriteEntry(stream, type.GetHandle()))
  {
    if (type.IsListType())
    {
      stream << "List<" << type.GetPlainTypeName() << '>';
    }
    else
    {
      stream << type.GetPlainTypeName();
    }
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

serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tType& dt)
{
  if (dt.IsListType())
  {
    stream << "List<" << dt.GetPlainTypeName() << '>';
  }
  else
  {
    stream << dt.GetPlainTypeName();
  }
  return stream;
}

serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tType& dt)
{
  std::string s;
  stream >> s;
  dt = tType::FindType(s);
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
