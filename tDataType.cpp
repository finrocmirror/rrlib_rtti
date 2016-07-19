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
#include "rrlib/rtti/tDataType.h"

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
static tDataType<uint8_t> cUINT8_TYPE("uint8");
static tDataType<uint16_t> cUINT16_TYPE("uint16");;
static tDataType<unsigned int> cUINT_TYPE("uint32");;
static tDataType<unsigned long long int> cULONGLONGINT_TYPE("uint64");;
static tDataType<int8_t> cINT8_TYPE("int8");
static tDataType<int16_t> cINT16_TYPE("int16");
static tDataType<int> cINT_TYPE("int32");
static tDataType<long long int> cLONGLONGINT_TYPE("int64");
static tDataType<double> cDOUBLE_TYPE("double");;
static tDataType<float> cFLOAT_TYPE("float");;
static tDataType<bool> cBOOL_TYPE("bool");;
static tDataType<rrlib::time::tTimestamp> cTIMESTAMP_TYPE("Timestamp");
static tDataType<rrlib::time::tDuration> cDURATION_TYPE("Duration");

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
