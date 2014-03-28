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
/*!\file    rrlib/rtti/tDataType.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"

namespace rrlib
{
namespace rtti
{

// some explicit template instantiations
template class tDataType<serialization::tMemoryBuffer>;
template class tDataType<int8_t>;
template class tDataType<int16_t>;
template class tDataType<int>;
template class tDataType<long int>;
template class tDataType<long long int>;
template class tDataType<uint8_t>;
template class tDataType<uint16_t>;
template class tDataType<unsigned int>;
template class tDataType<unsigned long int>;
template class tDataType<unsigned long long int>;
template class tDataType<double>;
template class tDataType<float>;
template class tDataType<bool>;
template class tDataType<std::string>;
template class tDataType<rrlib::time::tTimestamp>;
template class tDataType<rrlib::time::tDuration>;

static tDataType<std::string> cSTRING_TYPE("String"); // Make sure std::string data type has name "String" - as in Java

} // namespace rrlib
} // namespace rtti

