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
/*!\file    rrlib/rtti/tGenericObject.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-18
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/tGenericObject.h"

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

void tGenericObject::Deserialize(serialization::tInputStream& stream, serialization::tDataEncoding enc)
{
  if (enc == serialization::tDataEncoding::BINARY)
  {
    Deserialize(stream);
  }
  else if (enc == serialization::tDataEncoding::STRING)
  {
    serialization::tStringInputStream sis(stream.ReadString());
    Deserialize(sis);
  }
  else
  {
#ifdef _LIB_RRLIB_XML_PRESENT_
    std::string s = stream.ReadString();
    xml::tDocument d(s.c_str(), s.length(), false);
    xml::tNode& n = d.RootNode();
    Deserialize(n);
#else
    throw std::invalid_argument("XML support not available");
#endif
  }
}

void tGenericObject::Serialize(serialization::tOutputStream& stream, serialization::tDataEncoding enc) const
{
  if (enc == serialization::tDataEncoding::BINARY)
  {
    Serialize(stream);
  }
  else if (enc == serialization::tDataEncoding::STRING)
  {
    serialization::tStringOutputStream sos;
    Serialize(sos);
    stream.WriteString(sos.ToString());
  }
  else
  {
#ifdef _LIB_RRLIB_XML_PRESENT_
    xml::tDocument d;
    xml::tNode& n = d.AddRootNode("value");
    Serialize(n);
    stream.WriteString(n.GetXMLDump(true));
#else
    throw std::invalid_argument("XML support not available");
#endif
  }
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
