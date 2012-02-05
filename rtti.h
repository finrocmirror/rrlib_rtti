/**
 * You received this file as part of RRLib serialization
 *
 * Copyright (C) 2012 Max Reichardt,
 *   Robotics Research Lab, University of Kaiserslautern
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __rrlib__rtti__rtti_h__
#define __rrlib__rtti__rtti_h__

#include "rrlib/rtti/tFactory.h"
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
#include "rrlib/serialization/serialization.h"
#endif
#include "rrlib/rtti/sStaticTypeInfo.h"
#include "rrlib/rtti/tDataType.h"
#include "rrlib/rtti/serialization_extension.h"
#include "rrlib/rtti/detail/container_utils.h"

/*!
 * \author Max Reichardt
 *
 * This is the standard include for using rrlib_rtti.
 *
 * It contains various useful functions.
 */

namespace rrlib
{
namespace rtti
{

/*!
 * \param mangled Mangled type name as returned by RTTI
 * \return Demangled type name
 */
extern std::string Demangle(const char* mangled);

/*!
 * \return If a .so-file currently performs static initializations - returns name of .so file
 */
extern std::string GetBinaryCurrentlyPerformingStaticInitialization();

/*!
 * Resize vector (also works for vectors with noncopyable types)
 *
 * \param vector Vector to resize
 * \param new_size New Size
 */
template <typename T>
void ResizeVector(std::vector<T>& vector, size_t new_size)
{
  detail::tResize < std::vector<T>, T, !std::is_base_of<boost::noncopyable, T>::value >::Resize(vector, new_size);
}

} // namespace
} // namespace

#endif // __rrlib__rtti__rtti_h__
