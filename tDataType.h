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
/*!\file    rrlib/rtti/tDataType.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief   Contains tDataType
 *
 * \b tDataType
 *
 * Objects of this class contain and register runtime information about the
 * data type T.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tDataType_h__
#define __rrlib__rtti__tDataType_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <string>
#include <typeinfo>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"
#include "rrlib/rtti/type_traits.h"
#include "rrlib/rtti/tType.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
namespace serialization
{
class tInputStream;
class tOutputStream;
}
namespace rtti
{
class tGenericObject;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Runteim type information for T
/*!
 * Objects of this class contain and register runtime information about the
 * data type T.
 */
template<typename T>
class tDataType : public tType
{
  template <typename U = T>
  class tDataTypeInfo;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tDataType() : tType(GetDataTypeInfo())
  {
    this->GetElementType();
    this->GetListType();
    this->GetSharedPtrListType();
  }

  /*!
   * \param name Name data type should get (if different from default)
   */
  tDataType(const std::string& name) : tType(GetDataTypeInfo())
  {
    GetDataTypeInfo()->SetName(name);
    this->GetElementType();
    this->GetListType();
    this->GetSharedPtrListType();
  }

  /*!
   * Lookup data type by rtti name
   *
   * Tries T first
   * \param rtti_name rtti name
   * \return Data type with specified name (== NULL if it could not be found)
   */
  static tType FindTypeByRtti(const char* rtti_name)
  {
    if (rtti_name == GetDataTypeInfo()->rtti_name)
    {
      return tDataType();
    }
    return tType::FindTypeByRtti(rtti_name);
  }

  /*!
   * \return DataTypeInfo for this type T
   */
  static tInfo* GetDataTypeInfo()
  {
    static tDataTypeInfo<T> info;
    return &info;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*!
   * Data type info with factory functions
   */
  class tDataTypeInfoBase : public tType::tInfo
  {
  public:

    tDataTypeInfoBase();

    virtual tGenericObject* CreateInstanceGeneric(void* placement, bool emplace_generic_object) const override;

    virtual void DeepCopy(const void* src, void* dest, tFactory* f) const override;

    virtual void Deserialize(serialization::tInputStream& is, void* obj) const override;

    virtual void Serialize(serialization::tOutputStream& os, const void* obj) const override;
  };

  template <typename U>
  class tDataTypeInfo : public tDataTypeInfoBase
  {
  public:
    tDataTypeInfo()
    {
      this->type = tClassification::PLAIN;
      this->name = TypeName<T>::Get();
    }

    virtual void Init() override
    {
      AutoRegisterRelatedTypes<T>::Register();
    }
  };

  template <typename U>
  class tDataTypeInfo<std::vector<U>> : public tDataTypeInfoBase
  {
  public:
    tDataTypeInfo()
    {
      this->type = tClassification::LIST;
    }

    virtual void Init() override
    {
      this->element_type = tDataType<U>::GetDataTypeInfo();
      this->element_type->list_type = this;
      if (this->name.length() == 0)
      {
        this->name = std::string("List<") + this->element_type->name + ">";
      }
    }
  };
};

extern template class tDataType<serialization::tMemoryBuffer>;
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

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}

#include "rrlib/rtti/tDataType.hpp"

#endif
