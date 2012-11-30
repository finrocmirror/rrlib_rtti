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

#ifndef __rrlib__rtti__tDataType_h__
#define __rrlib__rtti__tDataType_h__

#include "rrlib/rtti/rtti.h"
#include "rrlib/rtti/type_traits.h"
#include "rrlib/rtti/tType.h"
#include "rrlib/rtti/detail/tListInfo.h"
#include <assert.h>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <cstring>

namespace rrlib
{
namespace serialization
{
class tInputStream;
class tOutputStream;
}
namespace rtti
{
class tGenericObject;
class tFactory;

/*!
 * \author Max Reichardt
 *
 * Objects of this class contain info about the data type T
 */
template<typename T>
class tDataType : public tType
{
  /*!
   * Data type info with factory functions
   */
  class tDataTypeInfo : public tType::tInfo
  {
  public:

    tDataTypeInfo();

    template <bool B>
    typename std::enable_if<B, tType::tInfo*>::type GetListTypeInfo()
    {
      return tDataType<typename detail::tListInfo<T>::tListType>::GetDataTypeInfo();
    }

    template <bool B>
    typename std::enable_if < !B, tType::tInfo* >::type GetListTypeInfo()
    {
      return NULL;
    }

    template <bool B>
    typename std::enable_if<B, tType::tInfo*>::type GetSharedPtrListTypeInfo()
    {
      return tDataType<typename detail::tListInfo<T>::tSharedPtrListType>::GetDataTypeInfo();
    }

    template <bool B>
    typename std::enable_if < !B, tType::tInfo* >::type GetSharedPtrListTypeInfo()
    {
      return NULL;
    }

    virtual void Init()
    {
      if (type == tClassification::PLAIN)
      {
        list_type = GetListTypeInfo<sStaticTypeInfo<T>::stl_container_suitable>();
        shared_ptr_list_type = GetSharedPtrListTypeInfo<sStaticTypeInfo<T>::shared_ptr_stl_container_suitable >();
      }
      else
      {
        element_type = tDataType<typename detail::tListInfo<T>::tElementType>::GetDataTypeInfo();
      }
    }

    virtual void* CreateInstance(void* placement) const
    {
      if (placement == NULL)
      {
        placement = operator new(sizeof(T));
      }
      memset(placement, 0, sizeof(T)); // set memory to 0 so that memcmp on class T can be performed cleanly for certain types
      return sStaticTypeInfo<T>::Create(placement);
    }

    virtual tGenericObject* CreateInstanceGeneric(void* placement) const;

    virtual void DeepCopy(const void* src, void* dest, tFactory* f) const;

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

    virtual void Deserialize(serialization::tInputStream& is, void* obj) const;

    virtual void Serialize(serialization::tOutputStream& os, const void* obj) const;

#endif

  };

public:
  tDataType() : tType(GetDataTypeInfo())
  {
    this->GetElementType();
    this->GetListType();
    this->GetSharedPtrListType();
  }

  // \param name Name data type should get (if different from default)
  tDataType(const std::string& name) : tType(GetDataTypeInfo())
  {
    GetDataTypeInfo()->SetName(name);
    this->GetElementType();
    this->GetListType();
    this->GetSharedPtrListType();
  }

  // Lookup data type by rtti name
  // Tries T first
  //
  // \param rtti_name rtti name
  // \return Data type with specified name (== NULL if it could not be found)
  static tType FindTypeByRtti(const char* rtti_name)
  {
    if (rtti_name == GetDataTypeInfo()->rtti_name)
    {
      return tDataType();
    }
    return tType::FindTypeByRtti(rtti_name);
  }

  // \return DataTypeInfo for this type T
  static tInfo* GetDataTypeInfo()
  {
    static tDataTypeInfo info;
    return &info;
  }

};

} // namespace
} // namespace

#include "rrlib/rtti/tDataType.hpp"

namespace rrlib
{
namespace rtti
{
template <>
class tDataType<detail::tNothing> : public tType
{
public:
  tDataType() : tType(NULL) {}
  static tInfo* GetDataTypeInfo()
  {
    return NULL;
  }
};

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
extern template class tDataType<serialization::tMemoryBuffer>;
#endif
extern template class tDataType<int8_t>;
extern template class tDataType<int16_t>;
extern template class tDataType<int>;
extern template class tDataType<long int>;
extern template class tDataType<long long int>;
extern template class tDataType<uint8_t>;
extern template class tDataType<uint16_t>;
extern template class tDataType<unsigned int>;
extern template class tDataType<unsigned long int>;
extern template class tDataType<unsigned long long int>;
extern template class tDataType<double>;
extern template class tDataType<float>;
extern template class tDataType<bool>;
extern template class tDataType<std::string>;
extern template class tDataType<rrlib::time::tTimestamp>;
extern template class tDataType<rrlib::time::tDuration>;

} // namespace
} // namespace

#endif // __rrlib__rtti__tDataType_h__
