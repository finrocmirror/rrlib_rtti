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
/*!\file    rrlib/rtti/rtti.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 * This is the standard include for using rrlib_rtti.
 *
 * It contains various useful functions.
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__rtti_h__
#define __rrlib__rtti__rtti_h__

#include "rrlib/serialization/serialization.h"
#include "rrlib/rtti/tDataType.h"


namespace rrlib
{
namespace rtti
{

/*!
 * \return If a .so-file currently performs static initializations - returns name of .so file
 */
std::string GetBinaryCurrentlyPerformingStaticInitialization();

/*!
 * Resize vector (also works for vectors with noncopyable types)
 *
 * \param vector Vector to resize
 * \param new_size New Size
 */
template <typename T>
void ResizeVector(std::vector<T>& vector, size_t new_size)
{
  serialization::ContainerResize<T>::Resize(vector, new_size);
}

} // namespace
} // namespace

#endif // __rrlib__rtti__rtti_h__
