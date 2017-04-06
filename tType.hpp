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
/*!\file    rrlib/rtti/tType2.hpp
 *
 * \author  Max Reichardt
 *
 * \date    2016-08-14
 *
 */
//----------------------------------------------------------------------

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


struct tGenericObjectDestructorCall
{
  void operator()(tGenericObject* object) const
  {
    object->~tGenericObject();
  }
};

tGenericObject* tType::CreateGenericObject() const
{
  void* placement = operator new(GetSize(true));
  char* object_address = static_cast<char*>(placement) + sizeof(tGenericObject);
  tGenericObject* generic_object = new(placement) tGenericObject(object_address, *this);
  EmplaceInstance(object_address);
  return generic_object;
}

tGenericObject* tType::CreateGenericObject(void* wrapped_data_placement) const
{
  char* object_address = static_cast<char*>(wrapped_data_placement);
  tGenericObject* generic_object = new tGenericObject(object_address, *this);
  EmplaceInstance(object_address);
  return generic_object;
}

inline std::unique_ptr<tGenericObject, tGenericObjectDestructorCall> tType::EmplaceGenericObject(void* placement) const
{
  char* object_address = static_cast<char*>(placement) + sizeof(tGenericObject);
  tGenericObject* generic_object = new(placement) tGenericObject(object_address, *this);
  EmplaceInstance(object_address);
  return std::unique_ptr<tGenericObject, tGenericObjectDestructorCall>(generic_object);
}

inline size_t tType::GetSize(bool as_generic_object) const
{
  return info->size + (as_generic_object ? sizeof(tGenericObject) : 0);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
