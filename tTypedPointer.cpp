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
/*!\file    rrlib/rtti/tTypedPointer.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2016-07-26
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/tTypedPointer.h"

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

void tTypedPointer::Deserialize(serialization::tStringInputStream& stream) const
{
  if (data && (type.GetTypeTraits() & trait_flags::cIS_STRING_SERIALIZABLE))
  {
    (*type.GetStringSerialization().deserialize)(stream, *this);
  }
  else
  {
    throw std::runtime_error("Deserialization not supported");
  }
}

void tTypedPointer::Deserialize(const xml::tNode& node) const
{
  if (data && (type.GetTypeTraits() & trait_flags::cIS_XML_SERIALIZABLE))
  {
#ifdef _LIB_RRLIB_XML_PRESENT_
    auto classification = type.GetTypeClassification();
    bool pair = type.GetTypeClassification() == tTypeClassification::PAIR;
    if (pair || classification == tTypeClassification::TUPLE)
    {
      auto tuple_elements = type.GetTupleTypes();
      auto it = node.ChildrenBegin();
      for (size_t i = 0; i < tuple_elements.second; i++, ++it)
      {
        if (it == node.ChildrenEnd())
        {
          throw std::runtime_error("Not enough XML siblings to de-serialize std::tuple of size " + std::to_string(tuple_elements.second) + " completely!");
        }
        const rrlib::xml::tNode& node = *it;
        tTypedPointer pointer(static_cast<char*>(data) + tuple_elements.first[i].offset, tType(tuple_elements.first[i].type_info));
        pointer.Deserialize(node);
      }
    }
    else if (type.GetTypeTraits() & trait_flags::cIS_STRING_SERIALIZABLE)
    {
      serialization::tStringInputStream stream(node.GetTextContent());
      Deserialize(stream);
    }
    else
    {
      (*type.GetXMLSerialization().deserialize)(node, *this);
    }
#endif
  }
  else
  {
    throw std::runtime_error("Deserialization not supported");
  }
}

void tTypedPointer::Deserialize(serialization::tInputStream& stream, serialization::tDataEncoding encoding) const
{
  if (encoding == serialization::tDataEncoding::BINARY)
  {
    Deserialize(stream);
  }
  else if (encoding == serialization::tDataEncoding::STRING)
  {
    serialization::tStringInputStream sis(stream.ReadString());
    Deserialize(sis);
  }
  else if (encoding == serialization::tDataEncoding::XML)
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

void tTypedConstPointer::Serialize(serialization::tStringOutputStream& stream) const
{
  if (data && (type.GetTypeTraits() & trait_flags::cIS_STRING_SERIALIZABLE))
  {
    (*type.GetStringSerialization().serialize)(stream, *this);
  }
  else
  {
    throw std::runtime_error("Serialization not supported");
  }
}

void tTypedConstPointer::Serialize(xml::tNode& node) const
{
  if (data && (type.GetTypeTraits() & trait_flags::cIS_XML_SERIALIZABLE))
  {
#ifdef _LIB_RRLIB_XML_PRESENT_
    auto classification = type.GetTypeClassification();
    bool pair = type.GetTypeClassification() == tTypeClassification::PAIR;
    if (pair || classification == tTypeClassification::TUPLE)
    {
      auto tuple_elements = type.GetTupleTypes();
      for (size_t i = 0; i < tuple_elements.second; i++)
      {
        rrlib::xml::tNode &child = node.AddChildNode(pair ? (i == 0 ? "first" : "second") : "tuple_element");
        tTypedConstPointer pointer(static_cast<const char*>(data) + tuple_elements.first[i].offset, tType(tuple_elements.first[i].type_info));
        pointer.Serialize(child);
      }
    }
    else if (type.GetTypeTraits() & trait_flags::cIS_STRING_SERIALIZABLE)
    {
      serialization::tStringOutputStream stream;
      Serialize(stream);
      node.SetContent(stream.ToString());
    }
    else
    {
      (*type.GetXMLSerialization().serialize)(node, *this);
    }
#endif
  }
  else
  {
    throw std::runtime_error("Serialization not supported");
  }
}

void tTypedConstPointer::Serialize(serialization::tOutputStream& stream, serialization::tDataEncoding encoding) const
{
  if (encoding == serialization::tDataEncoding::BINARY)
  {
    Serialize(stream);
  }
  else if (encoding == serialization::tDataEncoding::STRING)
  {
    serialization::tStringOutputStream sos;
    Serialize(sos);
    stream.WriteString(sos.ToString());
  }
  else if (encoding == serialization::tDataEncoding::XML)
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

std::string tTypedConstPointer::ToString() const
{
  if (!data)
  {
    return "<nullptr>";
  }
  if (type.GetTypeTraits() & trait_flags::cIS_STRING_SERIALIZABLE)
  {
    rrlib::serialization::tStringOutputStream stream;
    this->Serialize(stream);
    return stream.ToString();
  }
  std::ostringstream stream;
  stream << type.GetName() << " (at " << std::hex << data << ")";
  return stream.str();
}


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
