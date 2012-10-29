/**
 * You received this file as part of RRLib serialization
 *
 * Copyright (C) 2008-2011 Max Reichardt,
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
#include "rrlib/rtti/rtti.h"
#include "rrlib/rtti/tDataTypeAnnotation.h"
#include "rrlib/rtti/tDataTypeAnnotation.h"
#include "rrlib/logging/messages.h"

namespace rrlib
{
namespace rtti
{
tDataTypeBase::tDataTypeBase(tDataTypeInfoRaw* info_) :
  info(info_)
{
  if (info_ != NULL && info_->new_info == true)
  {
    std::unique_lock<std::recursive_mutex>(GetMutex());
    AddType(info_);
    info_->Init();
  }
}

void tDataTypeBase::AddType(tDataTypeInfoRaw* nfo)
{
  nfo->uid = static_cast<int16_t>(GetTypes().size());
  GetTypes().push_back(*this);
  nfo->new_info = false;
  std::string msg("Adding data type ");
  msg += GetName();
  RRLIB_LOG_PRINT(DEBUG_VERBOSE_1, msg);
}

void* tDataTypeBase::CreateInstance(void* placement) const
{
  if (info == NULL)
  {
    return NULL;
  }
  return info->CreateInstance(placement);
}

tDataTypeBase tDataTypeBase::FindType(const std::string& name)
{
  bool nulltype = strcmp(name.c_str(), "NULL") == 0;
  if (nulltype)
  {
    return GetNullType();
  }

  for (size_t i = 0u; i < GetTypes().size(); i++)
  {
    tDataTypeBase dt = GetTypes()[i];
    bool eq = name.compare(dt.GetName()) == 0;
    if (eq)
    {
      return dt;
    }
  }

  return tDataTypeBase(NULL);
}

std::string tDataTypeBase::GetDataTypeNameFromRtti(const char* rtti)
{
  std::string demangled = Demangle(rtti);

  // remove ::
  long int last_pos = -1;
  for (long int i = demangled.size() - 1; i >= 0; i--)
  {
    char c = demangled[i];
    if (last_pos == -1)
    {
      if (c == ':')
      {
        last_pos = i + 1;

        // possibly cut off s or t prefix
        if (islower(demangled[last_pos]) && isupper(demangled[last_pos + 1]))
        {
          last_pos++;
        }
      }
    }
    else
    {
      if ((!isalnum(c)) && c != ':' && c != '_')
      {
        // ok, cut off here
        demangled = demangled.substr(0, i + 1) + demangled.substr(last_pos, demangled.size() - last_pos);
        last_pos = -1;
      }
    }
  }

  // ok, cut off rest
  if (last_pos > 0)
  {
    demangled = demangled.substr(last_pos, demangled.size() - last_pos);
  }

  // possibly cut off s or t prefix
  if (islower(demangled[0]) && isupper(demangled[1]))
  {
    demangled.erase(0, 1);
  }

  return demangled;

}

size_t& tDataTypeBase::GetLastAnnotationIndex()
{
  static size_t last_index = 0;
  return last_index;
}

const char* tDataTypeBase::GetLogDescription()
{
  return "DataTypeBase";
}

std::recursive_mutex& tDataTypeBase::GetMutex()
{
  static std::recursive_mutex mutex;
  return mutex;
}

tDataTypeBase::tDataTypeInfoRaw::tDataTypeInfoRaw() :
  type(tType::UNKNOWN),
  name(),
  rtti_name(NULL),
  size(0),
  type_traits(0),
  new_info(true),
  default_name(true),
  uid(-1),
  element_type(NULL),
  list_type(NULL),
  shared_ptr_list_type(NULL),
  binary(),
  enum_strings(NULL)
{
  for (size_t i = 0; i < cMAX_ANNOTATIONS; i++)
  {
    annotations[i] = NULL;
  }
}

tDataTypeBase::tDataTypeInfoRaw::~tDataTypeInfoRaw()
{
  for (size_t i = 0; i < cMAX_ANNOTATIONS; i++)
  {
    delete annotations[i];
  }

}

void tDataTypeBase::tDataTypeInfoRaw::SetName(const std::string& new_name)
{
  if (!default_name)
  {
    assert(name.compare(new_name) == 0 && "Name already set");

    return;
  }

  default_name = false;
  name = new_name;

  if (list_type != NULL)
  {
    list_type->name = std::string("List<") + name + ">";
  }
  if (shared_ptr_list_type != NULL)
  {
    shared_ptr_list_type->name = std::string("List<") + name + "*>";
  }

}

#ifdef _LIB_RRLIB_SERIALIZATION_PRESENT_

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tDataTypeBase& dt)
{
  serialization::tTypeEncoding encoding = stream.GetTypeEncoding();
  if (encoding == serialization::tTypeEncoding::LOCAL_UIDS)
  {
    stream.WriteShort(dt.GetUid());
  }
  else if (encoding == serialization::tTypeEncoding::NAMES)
  {
    stream.WriteString(dt.GetName());
  }
  else
  {
    stream.GetCustomTypeEncoder()->WriteType(stream, dt);
  }
  return stream;
}

serialization::tInputStream& operator >> (serialization::tInputStream& stream, tDataTypeBase& dt)
{
  serialization::tTypeEncoding encoding = stream.GetTypeEncoding();
  if (encoding == serialization::tTypeEncoding::LOCAL_UIDS)
  {
    dt = tDataTypeBase::GetType(stream.ReadShort());
  }
  else if (encoding == serialization::tTypeEncoding::NAMES)
  {
    dt = tDataTypeBase::FindType(stream.ReadString());
  }
  else
  {
    dt = stream.GetCustomTypeEncoder()->ReadType(stream);
  }
  return stream;
}

serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tDataTypeBase& dt)
{
  stream << dt.GetName();
  return stream;
}

serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tDataTypeBase& dt)
{
  std::string s;
  stream >> s;
  dt = tDataTypeBase::FindType(s);
  return stream;
}

#endif


} // namespace
} // namespace

