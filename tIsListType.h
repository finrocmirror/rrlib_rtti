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
/*!\file    rrlib/rtti/tIsListType.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tIsListType_h__
#define __rrlib__rtti__tIsListType_h__


namespace rrlib
{
namespace rtti
{

/*!
 * \author Max Reichardt
 *
 * By deriving from this class, it can be indicated whether a class is suitable for use in lists.
 * When this is indicated, list types are automatically instantiated when class is registered as data type.
 *
 * BY_VALUE   indicates whether automatic creation of list type std::vector<T> is desired.
 * SHARED_PTR indicates whether automatic creation of list type std::vector<shared_ptr<T>> is desired.
 */

template <bool BY_VALUE, bool SHARED_PTR>
class tIsListType
{
};

} // namespace
} // namespace

#endif // __rrlib__rtti__tIsListType_h__
