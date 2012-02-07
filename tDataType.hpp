/**
 * You received this file as part of RRLib serialization
 *
 * Copyright (C) 2008-2012 Max Reichardt,
 *   Robotics Research Lab, University of Kaiserslautern
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "rrlib/rtti/tGenericObjectInstance.h"
#include "rrlib/rtti/type_traits.h"
#include "rrlib/logging/messages.h"

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
#include "rrlib/serialization/tSerializable.h"
#endif


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
  name = detail::tListInfo<T>::GetName();
  type_traits = tTypeTraitsVector<T>::value;
  binary = GetBinaryCurrentlyPerformingStaticInitialization();
  if (binary.length() > 0)
  {
    RRLIB_LOG_PRINT_STATIC(rrlib::logging::eLL_DEBUG_VERBOSE_1, "Data type ", name, " is statically loaded in '", binary, "'.");
  }
}

template<typename T>
tGenericObject* tDataType<T>::tDataTypeInfo::CreateInstanceGeneric(void* placement, size_t manager_size) const
{
  assert(sizeof(tGenericObjectInstance<T>) <= tGenericObject::cMANAGER_OFFSET);
  while (manager_size % 8 != 0)
  {
    manager_size++;
  }
  size_t obj_offset = tGenericObject::cMANAGER_OFFSET + manager_size;
  size_t size = obj_offset + sizeof(T);
  if (placement == NULL)
  {
    placement = operator new(size);
  }
  char* obj_addr = ((char*)placement) + obj_offset;
  memset(obj_addr, 0, sizeof(T)); // set memory to 0 so that memcmp on class T can be performed cleanly for certain types
  T* data_new = sStaticTypeInfo<T>::Create(obj_addr);
  return new(placement) tGenericObjectInstance<T>(data_new);
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

} // namespace
} // namespace

