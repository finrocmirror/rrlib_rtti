//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/rtti/tFactory.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tFactory_h__
#define __rrlib__rtti__tFactory_h__

#include <memory>

namespace rrlib
{
namespace rtti
{
class tType;
class tGenericObject;

/*!
 * \author Max Reichardt
 *
 * Factory to create objects (as shared_ptr) needed in input streams or
 * during deep copy operations.
 *
 * When deserializing pointer list, for example, buffers are needed.
 *
 * It may be specialized for more efficient buffer management.
 */
class tFactory
{
public:

  // Create buffer and place it in provided shared pointer
  template <typename T>
  void CreateBuffer(std::shared_ptr<T>& ptr, const tType& dt)
  {
    ptr = std::static_pointer_cast<T>(CreateBuffer(dt));
  }

  /*!
   * Create buffer
   * (used to fill vectors)
   *
   * \param dt Data type
   * \return Created buffer
   */
  virtual std::shared_ptr<void> CreateBuffer(const tType& dt) = 0;

  /*!
   * Create generic object
   * (used in writeObject() / readObject() of stream classes)
   *
   * \param dt Data type
   * \param factory_parameter Custom factory parameter
   * \return Created buffer
   */
  virtual tGenericObject* CreateGenericObject(const tType& dt, void* factory_parameter) = 0;

};

} // namespace
} // namespace

#endif // __rrlib__rtti__tFactory_h__
