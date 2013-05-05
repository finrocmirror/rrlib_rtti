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
#ifndef __rrlib__rtti__tGenericObjectWrapper_h__
#define __rrlib__rtti__tGenericObjectWrapper_h__

#include "rrlib/rtti/detail/tGenericObjectBaseImpl.h"
#include <assert.h>

namespace rrlib
{
namespace rtti
{
/*!
 * \author Max Reichardt
 *
 * Allows wrapping any object as GenericObject
 */
template < typename T >
class tGenericObjectWrapper : public detail::tGenericObjectBaseImpl<T>
{

public:
  tGenericObjectWrapper(T& wrapped_object) : detail::tGenericObjectBaseImpl<T>()
  {
    this->wrapped = &wrapped_object;
  }

};

} // namespace
} // namespace

#endif // __rrlib__rtti__tGenericObjectWrapper_h__
