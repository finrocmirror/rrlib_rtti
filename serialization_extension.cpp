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
#include "rrlib/rtti/rtti.h"

namespace rrlib
{
namespace rtti
{

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

static tDataType<serialization::tMemoryBuffer> init_memory_buffer_data_type;

tGenericObject* ReadObject(rrlib::serialization::tInputStream& stream, void* factory_parameter, serialization::tDataEncoding enc)
{
  //readSkipOffset();
  tType dt;
  stream >> dt;
  if (dt == NULL)
  {
    return NULL;
  }

  tGenericObject* buffer = stream.GetFactory()->CreateGenericObject(dt, factory_parameter);
  buffer->Deserialize(stream, enc);
  return buffer;
}

void WriteObject(rrlib::serialization::tOutputStream& stream, const tGenericObject* to, serialization::tDataEncoding enc)
{
  if (to == NULL)
  {
    stream << tType();
    return;
  }

  //writeSkipOffsetPlaceholder();
  stream << to->GetType();
  to->Serialize(stream, enc);
  //skipTargetHere();
}


#endif

} // namespace
} // namespace

