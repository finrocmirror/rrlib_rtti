/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
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

#ifndef __rrlib__rtti__tTypedObject_h__
#define __rrlib__rtti__tTypedObject_h__

#include "rrlib/rtti/tType.h"

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
#include "rrlib/serialization/tSerializable.h"
#endif

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
#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_
  : public serialization::tSerializable
#endif
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
