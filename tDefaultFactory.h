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
/*!\file    rrlib/rtti/tDefaultFactory.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tDefaultFactory_h__
#define __rrlib__rtti__tDefaultFactory_h__

#include "rrlib/rtti/tType.h"
#include "rrlib/rtti/tFactory.h"
#include "rrlib/util/tNoncopyable.h"
#include <memory>

namespace rrlib
{
namespace rtti
{
class tGenericObject;

/*!
 * \author Max Reichardt
 *
 * Default factory implementation.
 * Simply allocates and deletes objects as needed on Heap.
 */
class tDefaultFactory : public tFactory, private util::tNoncopyable
{
public:

  tDefaultFactory() {}
  virtual ~tDefaultFactory() {}

  virtual std::shared_ptr<void> CreateBuffer(const tType& dt) override
  {
    std::shared_ptr<tGenericObject> generic_object_pointer(dt.CreateInstanceGeneric()); // TODO: if this feature is heavily used, we could create a variant that avoids the two-pointer overhead from tGenericObjectInstance
    return std::shared_ptr<void>(generic_object_pointer, generic_object_pointer->GetRawDataPointer());
  }

  virtual tGenericObject* CreateGenericObject(const tType& dt, void* custom_parameter) override
  {
    return dt.CreateInstanceGeneric();
  }

};

} // namespace
} // namespace

#endif // __rrlib__rtti__tDefaultFactory_h__
