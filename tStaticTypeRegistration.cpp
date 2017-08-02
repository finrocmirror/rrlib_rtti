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
/*!\file    rrlib/rtti/tStaticTypeRegistration.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2017-08-02
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/tStaticTypeRegistration.h"

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

namespace
{
std::vector<tStaticTypeRegistration>& Register()
{
  static std::vector<tStaticTypeRegistration> instance;
  return instance;
}
}

tStaticTypeRegistration::tStaticTypeRegistration(const char* shared_library) :
  shared_library(shared_library)
{
  Register().push_back(*this);
}

void tStaticTypeRegistration::Add(const rrlib::rtti::tType& type)
{
  Register().back().types.push_back(type);
}

const char* tStaticTypeRegistration::GetTypeRegistrationSharedLibrary(const tType& type)
{
  for (tStaticTypeRegistration & reg : Register())
  {
    for (tType & registered_type : reg.types)
    {
      if (registered_type == type)
      {
        return reg.shared_library;
      }
    }
  }
  return nullptr;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
