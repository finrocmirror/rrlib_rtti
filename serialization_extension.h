//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__serialization_extension_h__
#define __rrlib__rtti__serialization_extension_h__

#include "rrlib/rtti/rtti.h"
#include "rrlib/rtti/tDefaultFactory.h"

/*!
 * \author Max Reichardt
 *
 * This header contains various enhancements to the rrlib_serializaiton library
 * such as serializing containers with shared pointers and writing generic
 * objects to streams.
 */

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

#include "rrlib/serialization/serialization.h"

namespace rrlib
{
namespace rtti
{
class tGenericObject;
class tType;

/*!
 * Deserialize object with yet unknown type from stream
 * (should have been written to stream with OutputStream.WriteObject() before; typeencoder should be of the same type)
 *
 * \param stream Stream to deserialize object from
 * \param factory_parameter Custom parameter for possibly user defined factory
 * \param enc Data encoding to use
 * \return Buffer with read object (caller needs to take care of deleting/recycling it)
 */
tGenericObject* ReadObject(rrlib::serialization::tInputStream& stream, void* factory_parameter, serialization::tDataEncoding enc = serialization::tDataEncoding::BINARY);

/*!
 * Serialize Object of arbitrary type to stream
 * (including type information)
 *
 * \param stream Stream to write to
 * \param to Object to write (may be null)
 */
void WriteObject(rrlib::serialization::tOutputStream& stream, const tGenericObject* to, serialization::tDataEncoding enc = serialization::tDataEncoding::BINARY);

} // namespace

namespace serialization
{

// The remainder of this file deals with STL container serialization
// It is not handled in rrlib_serialization, because of types that do not provide a default constructor

// Serialize STL container (must either have pass-by-value type or shared pointers)
template <typename C, typename T>
void WriteSTLContainer(tOutputStream& os, const C& container)
{
  os.WriteInt(container.size());
  os.WriteBoolean(true); // const type?
  typename C::const_iterator it;
  for (it = container.begin(); it != container.end(); it++)
  {
    os << *it;
  }
}

// Deserialize STL container (must have pass-by-value type)
template <typename C, typename T>
void ReadSTLContainer(tInputStream& is, C& container)
{
  size_t size = is.ReadInt();
  __attribute__((unused))
  bool const_type = is.ReadBoolean();
  assert(const_type && "This method only supports constant types");

  // container.resize(size);
  if (container.size() < size)
  {
    container.reserve(size);
  }
  while (container.size() < size)
  {
    container.push_back(rtti::sStaticTypeInfo<T>::CreateByValue());
  }
  while (container.size() > size)
  {
    container.pop_back();
  }

  typename C::iterator it;
  for (it = container.begin(); it != container.end(); it++)
  {
    is >> *it;
  }
}

template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tOutputStream>::type& operator<< (tOutputStream& os, const std::vector<T>& t)
{
  WriteSTLContainer<std::vector<T>, T>(os, t);
  return os;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tOutputStream>::type& operator<< (tOutputStream& os, const std::list<T>& t)
{
  WriteSTLContainer<std::list<T>, T>(os, t);
  return os;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tOutputStream>::type& operator<< (tOutputStream& os, const std::deque<T>& t)
{
  WriteSTLContainer<std::deque<T>, T>(os, t);
  return os;
}

template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tInputStream>::type& operator>> (tInputStream& is, std::vector<T>& t)
{
  ReadSTLContainer<std::vector<T>, T>(is, t);
  return is;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tInputStream>::type& operator>> (tInputStream& is, std::list<T>& t)
{
  ReadSTLContainer<std::list<T>, T>(is, t);
  return is;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tInputStream>::type& operator>> (tInputStream& is, std::deque<T>& t)
{
  ReadSTLContainer<std::deque<T>, T>(is, t);
  return is;
}

// shared pointer STL container serialization

template <typename C, typename T>
void WriteSmartPointerContainer(tOutputStream& os, const C& container)
{
  os.WriteInt(container.size());
  os.WriteBoolean(false); // const type?
  rtti::tType type_t = rtti::tDataType<T>();
  typename C::const_iterator it;
  for (it = container.begin(); it != container.end(); it++)
  {
    rtti::tType type;
    if (*it)
    {
      type = rtti::tDataType<T>::FindTypeByRtti(typeid(**it).name());
    }

    os << type;
    if (type == NULL)
    {
      continue;
    }
    if (type != type_t)
    {
      type.Serialize(os, it->get());
      continue;
    }
    os << **it;
  }
}

template <typename C, typename T>
void ReadSmartPointerContainer(tInputStream& is, C& container)
{
  size_t size = is.ReadInt();
  bool const_type = is.ReadBoolean();
  if (const_type)
  {
    throw std::runtime_error("Wrong list type");
  }

  container.resize(size);

  rtti::tType type_t = rtti::tDataType<T>();
  rtti::tDefaultFactory df;
  rtti::tFactory* f = is.GetFactory() ? is.GetFactory() : static_cast<rtti::tFactory*>(&df);
  typename C::iterator it;
  for (it = container.begin(); it != container.end(); it++)
  {
    rtti::tType needed;
    is >> needed;
    rtti::tType current;
    if (*it)
    {
      current = rtti::tDataType<T>::FindTypeByRtti(typeid(**it).name());
    }

    if (needed != current)
    {
      if (needed == NULL)
      {
        it->reset();
      }
      else
      {
        f->CreateBuffer(*it, needed);
      }
    }
    if (needed != type_t)
    {
      needed.Deserialize(is, it->get());
      continue;
    }
    is >> **it;
  }
}

template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tOutputStream>::type& operator<< (tOutputStream& os, const std::vector<std::shared_ptr<T>>& t)
{
  WriteSmartPointerContainer<std::vector<std::shared_ptr<T>>, T>(os, t);
  return os;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tOutputStream>::type& operator<< (tOutputStream& os, const std::list<std::shared_ptr<T>>& t)
{
  WriteSmartPointerContainer<std::list<std::shared_ptr<T>>, T>(os, t);
  return os;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tOutputStream>::type& operator<< (tOutputStream& os, const std::deque<std::shared_ptr<T>>& t)
{
  WriteSmartPointerContainer<std::deque<std::shared_ptr<T>>, T>(os, t);
  return os;
}

template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tInputStream>::type& operator>> (tInputStream& is, std::vector<std::shared_ptr<T>>& t)
{
  ReadSmartPointerContainer<std::vector<std::shared_ptr<T>>, T>(is, t);
  return is;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tInputStream>::type& operator>> (tInputStream& is, std::list<std::shared_ptr<T>>& t)
{
  ReadSmartPointerContainer<std::list<std::shared_ptr<T>>, T>(is, t);
  return is;
}
template <typename T, bool ENABLE = tIsBinarySerializable<T>::value>
inline typename std::enable_if<ENABLE, tInputStream>::type& operator>> (tInputStream& is, std::deque<std::shared_ptr<T>>& t)
{
  ReadSmartPointerContainer<std::deque<std::shared_ptr<T>>, T>(is, t);
  return is;
}

} // namespace
namespace xml
{

// for std::vector<bool> support
inline const tNode& operator>> (const tNode& node, std::vector<bool>::reference br)
{
  bool b;
  node >> b;
  br = b;
  return node;
}

template <typename T, bool ENABLE = rrlib::serialization::tIsXMLSerializable<T>::value>
inline typename std::enable_if<ENABLE, tNode>::type& operator<< (tNode& node, const std::vector<T>& v)
{
  for (size_t i = 0; i < v.size(); i++)
  {
    tNode& enode = node.AddChildNode("element");
    enode << v[i];
  }
  return node;
}

template <typename T, bool ENABLE = rrlib::serialization::tIsXMLSerializable<T>::value>
inline typename std::enable_if<ENABLE, tNode>::type& operator<< (tNode& node, const std::vector<std::shared_ptr<T>>& v)
{
  for (size_t i = 0; i < v.size(); i++)
  {
    tNode& enode = node.AddChildNode("element");
    enode << *v[i];
  }
  return node;
}

template <typename T, bool ENABLE = rrlib::serialization::tIsXMLSerializable<T>::value>
inline const typename std::enable_if<ENABLE, tNode>::type& operator>> (const tNode& n, std::vector<T>& v)
{
  v.clear();
  for (tNode::const_iterator node = n.ChildrenBegin(); node != n.ChildrenEnd(); ++node)
  {
    if (node->Name().compare("element") == 0)
    {
      v.push_back(rtti::sStaticTypeInfo<T>::CreateByValue());
      (*node) >> v[v.size() - 1];
    }
  }
  return n;
}

template <typename T, bool ENABLE = rrlib::serialization::tIsXMLSerializable<T>::value>
inline const typename std::enable_if<ENABLE, tNode>::type& operator>> (const tNode& n, std::vector<std::shared_ptr<T>>& v)
{
  v.clear();
  rtti::tDefaultFactory df;
  rtti::tDataType<T> dt;
  for (tNode::const_iterator node = n.ChildrenBegin(); node != n.ChildrenEnd(); ++node)
  {
    if (node->Name().compare("element") == 0)
    {
      v.push_back(NULL);
      static_cast<rtti::tFactory&>(df).CreateBuffer(v[v.size() - 1], dt);
      (*node) >> *v[v.size() - 1];
    }
  }
  return n;
}

} // namespace
} // namespace

#endif

#endif // __rrlib__rtti__serialization_extension_h__
