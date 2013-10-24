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
/*!\file    rrlib/rtti/tTypedObject.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__tTypedObject_h__
#define __rrlib__rtti__tTypedObject_h__

#include "rrlib/rtti/tType.h"


namespace rrlib
{
namespace rtti
{
/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for any object that has additional
 * type information as provided in this package.
 *
 * If rrlib_serialization is present, such classes can be cleanly serialized to
 * streams.
 */
class tTypedObject
{
protected:

  /*! Type of object */
  tType type;

public:

  /*!
   * \return Type of object
   */
  tTypedObject() :
    type()
  {}

  virtual ~tTypedObject() {}

  /*!
   * \return Log description (default implementation is "<class name> (<pointer>)"
   */
  inline const tTypedObject& GetLogDescription() const
  {
    return *this;
  }

  inline tType GetType() const
  {
    return type;
  }

};

inline std::ostream& operator << (std::ostream& output, const tTypedObject* lu)
{
  output << typeid(*lu).name() << " (" << ((void*)lu) << ")";
  return output;
}
inline std::ostream& operator << (std::ostream& output, const tTypedObject& lu)
{
  output << (&lu);
  return output;
}

} // namespace
} // namespace

#endif // __rrlib__rtti__tTypedObject_h__
