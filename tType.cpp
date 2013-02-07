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
/*!\file    rrlib/rtti/tType.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-11-02
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/tType.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <mutex>
#include "rrlib/logging/messages.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"
#include "rrlib/rtti/tTypeAnnotation.h"

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
namespace internal
{
static std::recursive_mutex& GetMutex()
{
  static std::recursive_mutex mutex;
  return mutex;
}

/*!
 * Helper method that safely provides static data type list
 */
static std::vector<tType>& GetTypes()
{
  static std::vector<tType> types;
  return types;
}

/*!
 * Helper for AddAnnotation (because we cannot store static variable in AddAnnotation that is the same for all types T)
 */
static size_t& GetLastAnnotationIndex()
{
  static size_t last_index = 0;
  return last_index;
}

} // namespace internal


tType::tType(tType::tInfo* info) :
  info(info)
{
  if (info && info->new_info)
  {
    // Add data type to registry
    std::unique_lock<std::recursive_mutex> lock(internal::GetMutex());
    info->uid = static_cast<int16_t>(internal::GetTypes().size());
    internal::GetTypes().push_back(*this);
    info->new_info = false;
    RRLIB_LOG_PRINT(DEBUG_VERBOSE_1, "Adding data type ", GetName());

    info->Init();
  }
}

void tType::AddAnnotationImplementation(tTypeAnnotation* annotation, bool (*annotation_index_valid_function)(bool), int& annotation_index)
{
  if (!annotation)
  {
    RRLIB_LOG_PRINT(ERROR, "NULL object passed to function. Ignoring.");
    return;
  }
  if (annotation->annotated_type != NULL)
  {
    RRLIB_LOG_PRINT(ERROR, "Annotation already added to other data type. This is not allowed (double deleting etc.). Ignoring.");
    return;
  }

  std::unique_lock<std::recursive_mutex> lock(internal::GetMutex());
  static size_t& last_annotation_index = internal::GetLastAnnotationIndex();
  if (info)
  {
    annotation->annotated_type = *this;
    size_t ann_index = -1u;

    if (!(*annotation_index_valid_function)(false))
    {
      last_annotation_index++;
      if (last_annotation_index >= cMAX_ANNOTATIONS)
      {
        RRLIB_LOG_PRINT(ERROR, "Maximum number of annotations (tType::cMAX_ANNOTATIONS) exceeded. Aborting.");
        abort();
      }
      annotation_index = last_annotation_index;
      (*annotation_index_valid_function)(true);
    }
    ann_index = annotation_index;

    assert(ann_index > 0 && ann_index < cMAX_ANNOTATIONS);

    if (info->annotations[ann_index])
    {
      RRLIB_LOG_PRINT(ERROR, "An annotation with this type was already to data type '", GetName(), "'. Aborting.");
      abort();
    }

    const_cast<tInfo*>(info)->annotations[ann_index] = annotation;
  }
  else
  {
    RRLIB_LOG_PRINT(ERROR, "Cannot add annotation to NULL type. Ignoring.");
  }
}

void tType::DeepCopy(const void* source, void* destination, tFactory* factory) const
{
  if (info)
  {
    info->DeepCopy(source, destination, factory);
  }
  else
  {
    RRLIB_LOG_PRINT(ERROR, "Cannot deep copy using NULL type.");
  }
}

tType tType::FindType(const std::string& name)
{
  if (name.compare("NULL") == 0)
  {
    return tType();
  }

  for (auto it = internal::GetTypes().begin(); it != internal::GetTypes().end(); ++it)
  {
    if (name.compare(it->info->name) == 0)
    {
      return *it;
    }
  }

  return tType();
}

tType tType::FindTypeByRtti(const char* rtti_name)
{
  for (auto it = internal::GetTypes().begin(); it != internal::GetTypes().end(); ++it)
  {
    if (it->info->rtti_name == rtti_name)
    {
      return *it;
    }
  }
  return tType();
}

std::string tType::GetDataTypeNameFromRtti(const char* rtti)
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

tType tType::GetType(int16_t uid)
{
  if (uid <= -1 || static_cast<int>(uid) >= internal::GetTypes().size())
  {
    return tType();
  }
  return internal::GetTypes()[uid];
}

uint16_t tType::GetTypeCount()
{
  return static_cast<uint16_t>(internal::GetTypes().size());
}

tType::tInfo::tInfo() :
  type(tClassification::UNKNOWN),
  name(),
  rtti_name(NULL),
  size(0),
  generic_object_size(0),
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

tType::tInfo::~tInfo()
{
  for (size_t i = 0; i < cMAX_ANNOTATIONS; i++)
  {
    delete annotations[i];
  }
}

void tType::tInfo::DeepCopy(const void* src, void* dest, tFactory* f) const
{
  RRLIB_LOG_PRINT(ERROR, "Not implemented for this type");
}

void tType::tInfo::SetName(const std::string& new_name)
{
  if (!default_name)
  {
    if (new_name.compare(name) != 0)
    {
      RRLIB_LOG_PRINT(ERROR, "Name has already been set to '", name, "'. Ignoring.");
    }
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

void tType::Deserialize(serialization::tInputStream& is, void* obj) const
{
  if (info == NULL)
  {
    return;
  }
  info->Deserialize(is, obj);
}

void tType::Serialize(serialization::tOutputStream& os, const void* obj) const
{
  if (info == NULL)
  {
    return;
  }
  info->Serialize(os, obj);
}

void tType::tInfo::Deserialize(serialization::tInputStream& is, void* obj) const
{
  RRLIB_LOG_PRINT(ERROR, "Not implemented for this type");
}

void tType::tInfo::Serialize(serialization::tOutputStream& os, const void* obj) const
{
  RRLIB_LOG_PRINT(ERROR, "Not implemented for this type");
}


serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tType& dt)
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

serialization::tInputStream& operator >> (serialization::tInputStream& stream, tType& dt)
{
  serialization::tTypeEncoding encoding = stream.GetTypeEncoding();
  if (encoding == serialization::tTypeEncoding::LOCAL_UIDS)
  {
    dt = tType::GetType(stream.ReadShort());
  }
  else if (encoding == serialization::tTypeEncoding::NAMES)
  {
    dt = tType::FindType(stream.ReadString());
  }
  else
  {
    dt = stream.GetCustomTypeEncoder()->ReadType(stream);
  }
  return stream;
}

serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tType& dt)
{
  stream << dt.GetName();
  return stream;
}

serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tType& dt)
{
  std::string s;
  stream >> s;
  dt = tType::FindType(s);
  return stream;
}

#endif


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
