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
/*!\file    rrlib/rtti/tDataType.hpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/tGenericObjectInstance.h"
#include "rrlib/rtti/tGenericObjectWrapper.h"
#include "rrlib/rtti/type_traits.h"
#include "rrlib/logging/messages.h"


namespace rrlib
{
namespace rtti
{

std::string GetBinaryCurrentlyPerformingStaticInitialization();

template<typename T>
tDataType<T>::tDataTypeInfo::tDataTypeInfo()
{
  type = detail::tListInfo<T>::type;
  rtti_name = typeid(T).name();
  size = sizeof(T);
  generic_object_size = sizeof(tGenericObjectInstance<T>);
  name = detail::tListInfo<T>::GetName();
  type_traits = tTypeTraitsVector<T>::value;
  binary = GetBinaryCurrentlyPerformingStaticInitialization();
  if (binary.length() > 0)
  {
    RRLIB_LOG_PRINT_STATIC(DEBUG_VERBOSE_1, "Data type ", name, " is statically loaded in '", binary, "'.");
  }
  if (std::is_enum<T>::value)
  {
    this->enum_strings = make_builder::GetEnumStrings<T>();
    this->enum_strings_dimension = make_builder::GetEnumStringsDimension<T>();
  }
}

template<typename T>
tGenericObject* tDataType<T>::tDataTypeInfo::CreateInstanceGeneric(void* placement) const
{
  if (placement == NULL)
  {
    placement = operator new(sizeof(tGenericObjectInstance<T>));
  }
  memset(placement, 0, sizeof(tGenericObjectInstance<T>)); // set memory to 0 so that memcmp on class T can be performed cleanly for certain types
  return new(placement) tGenericObjectInstance<T>();
}

template<typename T>
void tDataType<T>::tDataTypeInfo::DeepCopy(const void* src, void* dest, tFactory* f) const
{
  const T* s = static_cast<const T*>(src);
  T* d = static_cast<T*>(dest);

  if (std::has_virtual_destructor<T>::value)
  {
    assert(typeid(*s).name() == typeid(T).name());
    assert(typeid(*d).name() == typeid(T).name());
  }

  sStaticTypeInfo<T>::DeepCopy(*s, *d, f);
}

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

template<typename T>
void tDataType<T>::tDataTypeInfo::Deserialize(serialization::tInputStream& is, void* obj) const
{
  T* s = static_cast<T*>(obj);
  if (std::has_virtual_destructor<T>::value)
  {
    assert(typeid(*s).name() == typeid(T).name());
  }
  serialization::Deserialize(is, *s);
}

template<typename T>
void tDataType<T>::tDataTypeInfo::Serialize(serialization::tOutputStream& os, const void* obj) const
{
  const T* s = static_cast<const T*>(obj);
  if (std::has_virtual_destructor<T>::value)
  {
    assert(typeid(*s).name() == typeid(T).name());
  }
  serialization::Serialize(os, *s);
}

#endif

} // namespace
} // namespace

