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
/*!\file    rrlib/rtti/detail/tDataType.hpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 */
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/logging/messages.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tGenericObjectInstance.h"
#include "rrlib/rtti/tGenericObjectWrapper.h"
#include "rrlib/rtti/detail/tGenericObjectInstanceEmplaced.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

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
std::string GetBinaryCurrentlyPerformingStaticInitialization();

namespace detail
{

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

template<typename T>
tDataType<T>::tDataTypeInfoBase::tDataTypeInfoBase(tType::tClassification classification, const std::string& name) :
  tInfo(classification, typeid(T).name(), name)
{
  size = sizeof(T);
  generic_object_size = sizeof(tGenericObjectInstance<T>);
  type_traits = tTypeTraitsVector<T>::value;
  binary = GetBinaryCurrentlyPerformingStaticInitialization();
#if RRLIB_RTTI_BINARY_DETECTION_ENABLED
  if (binary.length() > 0)
  {
    RRLIB_LOG_PRINT_STATIC(DEBUG_VERBOSE_1, "Data type ", name, " is statically loaded in '", binary, "'.");
  }
  else
  {
    RRLIB_LOG_PRINT_STATIC(DEBUG_VERBOSE_1, "Data type ", name, " is dynamically loaded.");
  }
#endif
}

template<typename T>
tGenericObject* tDataType<T>::tDataTypeInfoBase::CreateInstanceGeneric(void* placement, bool emplace_generic_object) const
{
  if (placement == NULL)
  {
    placement = operator new(emplace_generic_object ? sizeof(tGenericObjectInstance<T>) : sizeof(T));
  }
  memset(placement, 0, emplace_generic_object ? sizeof(tGenericObjectInstance<T>) : sizeof(T)); // set memory to 0 so that memcmp on class T can be performed cleanly for certain types
  if (emplace_generic_object)
  {
    return new(placement) tGenericObjectInstance<T>();
  }
  else
  {
    return new detail::tGenericObjectInstanceEmplaced<T>(placement);
  }
}

template<typename T>
void tDataType<T>::tDataTypeInfoBase::DeepCopy(const void* src, void* dest, tFactory* f) const
{
  const T* s = static_cast<const T*>(src);
  T* d = static_cast<T*>(dest);

  if (std::has_virtual_destructor<T>::value)
  {
    assert(typeid(*s).name() == typeid(T).name());
    assert(typeid(*d).name() == typeid(T).name());
  }

  GenericOperations<T>::DeepCopy(*s, *d);
}

template<typename T>
void tDataType<T>::tDataTypeInfoBase::Deserialize(serialization::tInputStream& is, void* obj) const
{
  T* s = static_cast<T*>(obj);
  if (std::has_virtual_destructor<T>::value)
  {
    assert(typeid(*s).name() == typeid(T).name());
  }
  serialization::Deserialize(is, *s);
}

template<typename T>
void tDataType<T>::tDataTypeInfoBase::Serialize(serialization::tOutputStream& os, const void* obj) const
{
  const T* s = static_cast<const T*>(obj);
  if (std::has_virtual_destructor<T>::value)
  {
    assert(typeid(*s).name() == typeid(T).name());
  }
  serialization::Serialize(os, *s);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
