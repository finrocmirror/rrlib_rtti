/**
 * You received this file as part of RRLib serialization
 *
 * Copyright (C) 2011 Max Reichardt,
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

#ifndef __rrlib__rtti__tDataTypeAnnotation_h__
#define __rrlib__rtti__tDataTypeAnnotation_h__

#include "rrlib/rtti/tDataTypeBase.h"
#include <boost/utility.hpp>

namespace rrlib
{
namespace rtti
{
/*!
 * \author Max Reichardt
 *
 * Custom annotations for data types should be derived from this class
 */
class tDataTypeAnnotation : public boost::noncopyable
{
public:

  /*! Type that is annotated */
  tDataTypeBase annotated_type;

  tDataTypeAnnotation() :
    annotated_type()
  {}

  virtual ~tDataTypeAnnotation() {}

  /*!
   * \return Type that is annotated
   */
  inline tDataTypeBase GetAnnotatedType()
  {
    return annotated_type;
  }

};

} // namespace
} // namespace

#endif // __rrlib__rtti__tDataTypeAnnotation_h__
