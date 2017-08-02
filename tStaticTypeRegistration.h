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
/*!\file    rrlib/rtti/tStaticTypeRegistration.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-08-02
 *
 * \brief   Contains tStaticTypeRegistration
 *
 * \b tStaticTypeRegistration
 *
 * Helper class to statically register types in shared libraries
 * (typically in a file rtti.cpp)
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tStaticTypeRegistration_h__
#define __rrlib__rtti__tStaticTypeRegistration_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tType.h"

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
//! Helper class for static type registration
/*!
 * Helper class to statically register types in shared libraries
 * (typically in a file rtti.cpp)
 */
class tStaticTypeRegistration
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \param shared_library Shared library in which this static type registration is instantiated (in normalized form: e.g. rrlib_rtti)
   */
  tStaticTypeRegistration(const char* shared_library);

  /*!
   * Registers type with default name
   */
  template <typename T>
  inline tStaticTypeRegistration& Add()
  {
    tDataType<T> type;
    Add(type);
    return *this;
  }

  /*!
   * Registers type with custom name
   *
   * \param primary_name Custom name
   */
  template <typename T, size_t Tchars>
  tStaticTypeRegistration& Add(const char(&primary_name)[Tchars])
  {
    tDataType<T> type(primary_name);
    Add(type);
    return *this;
  }

  /*!
   * Registers type with two custom names
   *
   * \param primary_name Primary custom name
   * \param legacy_name Secondary custom name (typically legacy name)
   */
  template <typename T, size_t Tchars>
  tStaticTypeRegistration& Add(const char(&primary_name)[Tchars], const char* legacy_name)
  {
    tDataType<T> type(primary_name);
    type.AddName(legacy_name);
    Add(type);
    return *this;
  }

  static const char* GetTypeRegistrationSharedLibrary(const tType& type);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Shared library in which this static type registration is instantiated (in normalized form: e.g. rrlib_rtti) */
  const char* shared_library;

  /*! Types that were registered */
  std::vector<tType> types;

  /*! Internal add function */
  void Add(const rrlib::rtti::tType& type);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
