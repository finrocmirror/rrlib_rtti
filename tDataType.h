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

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/detail/tDataTypeInfo.h"

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
// Class declaration
//----------------------------------------------------------------------
//! Runtime type information for T
/*!
 * Objects of this class contain and register runtime information about the
 * data type T.
 */
template<typename T>
class tDataType : public tType
{
  typedef typename NormalizedType<T>::type tNormalizedType;
  typedef detail::tDataTypeInfo<tNormalizedType> tInfo;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  constexpr tDataType() : tType(&tInfo::value.data_type_info)
  {
  }

  /*!
   * Custom name may only be specified on first instantiation of a tDataType<T> for each type T
   *
   * \param name Name data type should get (if different from default). Only string literals should be passed to this functions.
   */
  template <size_t Tchars>
  tDataType(const char(&name)[Tchars]) : tType(&tInfo::value.data_type_info)
  {
    GetSharedInfo().SetName(util::tManagedConstCharPointer(name, false), &tInfo::value.data_type_info);
  }
};

// Types defined in this library
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

#endif
