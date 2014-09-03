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
#include <set>
#include <unordered_map>
#include "rrlib/logging/messages.h"
#include "rrlib/util/demangle.h"

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

/*! we need to avoid reallocation in order to make vector thread safe. Therefore it is created with this capacity. */
const size_t cMAX_TYPES = 1000;

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

static std::vector<tType>& GetTypeVector()
{
  static std::vector<tType> types;
  types.reserve(cMAX_TYPES); // we need to avoid reallocation in order to make vector thread safe
  return types;
}

/*!
 * Helper method that safely provides static data type list
 */
static std::vector<tType>& GetTypes()
{
  static std::vector<tType>& types = GetTypeVector(); // we introduce an extra method to avoid doing or checking for reserve() on every call
  assert(types.capacity() >= cMAX_TYPES);
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

/*!
 * Comparator functor for desired order in set returned by GetRenamedTypes()
 */
struct RenamedTypeCompare
{
  bool operator()(tType lhs, tType rhs) const
  {
    return lhs.GetRttiNameDemangled().length() > rhs.GetRttiNameDemangled().length();
  }
};

/*!
 * \return Set with all types that have been renamed (sorted by length of original demangled name - longest first)
 */
static std::set<tType, RenamedTypeCompare>& GetRenamedTypes()
{
  static std::set<tType, RenamedTypeCompare> renamed_types;
  return renamed_types;
}

/*!
 * Replaces std container names in string that is provided
 *
 * \param type_name Type name string to modify
 */
static void ReplaceContainerNames(std::string& type_name)
{
  static const std::unordered_map<std::string, std::string> cTEMPLATE_NAMES = { {"std::vector<", "List"}, {"std::set<", "Set"}, {"std::map<", "Map"}, {"std::tuple<", "Tuple"} };
  static const std::string cREMOVE_LAST_ARG = "std::allocator<";

  for (auto & entry : cTEMPLATE_NAMES)
  {
    bool word_start = true;
    for (size_t i = 0; i < type_name.length(); i++)
    {
      if (word_start && type_name.compare(i, entry.first.length(), entry.first) == 0)
      {
        // search for end of template
        size_t bracket_count = 1;
        size_t last_template_argument_start = 0;
        size_t last_index = 0;
        for (size_t index = i + entry.first.length(); index < type_name.length(); index++)
        {
          if (type_name[index] == '<')
          {
            bracket_count++;
          }
          else if (type_name[index] == '>')
          {
            bracket_count--;
            if (bracket_count == 0)
            {
              last_index = index;
              break;
            }
          }
          else if (bracket_count == 1 && type_name[index] == ',')
          {
            last_template_argument_start = index;
          }
        }

        if (last_template_argument_start && last_index)
        {
          // Remove last template argument?
          if (type_name.compare(last_template_argument_start + 1, cREMOVE_LAST_ARG.length(), cREMOVE_LAST_ARG) == 0 ||
              type_name.compare(last_template_argument_start + 2, cREMOVE_LAST_ARG.length(), cREMOVE_LAST_ARG) == 0)
          {
            type_name.replace(last_template_argument_start, last_index - last_template_argument_start, "");
          }

          // Replace template name
          type_name.replace(i, entry.first.length() - 1, entry.second);

          i = 0;
          word_start = true;
          continue;
        }
      }
      word_start = !isalnum(type_name[i]);
    }
  }
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
    if (internal::GetTypes().size() >= cMAX_TYPES)
    {
      RRLIB_LOG_PRINT(ERROR, "Maximum number of data types exceeded. Increase cMAX_TYPES.");
      throw std::runtime_error("Maximum number of data types exceeded. Increase cMAX_TYPES.");
    }
    internal::GetTypes().push_back(*this);
    info->new_info = false;
    RRLIB_LOG_PRINT(DEBUG_VERBOSE_1, "Adding data type ", GetName());

    // Add to string replacement table?
    if (GetTypeNameFromRtti(info->rtti_name) != info->name)
    {
      internal::GetRenamedTypes().insert(*this);

      // Debug output
      /*RRLIB_LOG_PRINT(DEBUG, "Content:");
      for (auto it = internal::GetRenamedTypes().begin(); it != internal::GetRenamedTypes().end(); ++it)
      {
        RRLIB_LOG_PRINT(DEBUG, it->info->demangled_rtti_name);
      }*/
    }

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

  if (name.find('.') != std::string::npos) // namespaces in specified name
  {
    return FindType(RemoveNamespaces(name));
  }
  else // no namespace in specified name
  {
    for (auto it = internal::GetTypes().begin(); it != internal::GetTypes().end(); ++it)
    {
      if (name.compare(it->info->short_name) == 0)
      {
        return *it;
      }
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

std::string tType::GetTypeNameFromRtti(const char* rtti, bool remove_namespaces)
{
  std::string demangled = util::Demangle(rtti);

  // Do we have a template? => check for string replacements
  if (demangled.find('<') != std::string::npos)
  {
    internal::ReplaceContainerNames(demangled);

    // TODO: std::regex might be more convenient (available on all relevant platforms?)
    for (auto it = internal::GetRenamedTypes().begin(); it != internal::GetRenamedTypes().end(); ++it)
    {
      size_t found_index = 0;

      while ((found_index = demangled.find(it->GetRttiNameDemangled(), found_index + 1)) != std::string::npos)
      {
        // we only want to replace whole words => character before and after must not be a letter
        if ((!isalnum(demangled[found_index - 1])) && found_index + it->GetRttiNameDemangled().length() < demangled.length() &&
            (!isalnum(demangled[found_index + it->GetRttiNameDemangled().length()])))
        {
          demangled.replace(found_index, it->GetRttiNameDemangled().length(), it->GetName());
          found_index = 0;
        }
      }
    }
  }

  // Replace '::' with '.', remove 't' prefixes, replace '> >' with '>>'
  bool word_start = true;
  char name[demangled.size() + 1];
  char* name_ptr = name;
  memset(name, 0, demangled.size() + 1);

  for (size_t i = 0; i < demangled.size(); i++)
  {
    if (i + 1 < demangled.size())
    {
      if (demangled[i] == ':' && demangled[i + 1] == ':')
      {
        *name_ptr = '.';
        name_ptr++;
        i++;
        word_start = true;
        continue;
      }
      if (word_start && demangled[i] == 't' && isupper(demangled[i + 1]))
      {
        i++;
      }
    }

    *name_ptr = demangled[i];
    name_ptr++;
    word_start = !isalnum(demangled[i]);

    if (demangled.compare(i, 3, "> >") == 0)
    {
      i++; // Skip space
    }
  }

  if (remove_namespaces)
  {
    return RemoveNamespaces(name);
  }

  return name;
}

tType tType::GetType(int16_t uid)
{
  if (uid <= -1 || static_cast<size_t>(uid) >= internal::GetTypes().size())
  {
    return tType();
  }
  return internal::GetTypes()[uid];
}

uint16_t tType::GetTypeCount()
{
  return static_cast<uint16_t>(internal::GetTypes().size());
}

std::string tType::RemoveNamespaces(const std::string& type_name)
{
  char result[type_name.length() + 1];
  char* result_ptr = &result[type_name.length()];
  memset(result, 0, type_name.length() + 1);
  bool in_namespace = false;

  for (auto it = type_name.rbegin(); it != type_name.rend(); ++it)
  {
    if (*it == '.')
    {
      in_namespace = true;
    }
    if (*it == ',' || *it == '<' || *it == ' ')
    {
      in_namespace = false;
    }
    if (!in_namespace)
    {
      result_ptr--;
      assert(result_ptr >= result);
      *result_ptr = *it;
    }
  }

  RRLIB_LOG_PRINT(DEBUG_VERBOSE_2, "Input: ", type_name, " Output: ", result_ptr);
  return result_ptr;
}

tType::tInfo::tInfo(tType::tClassification classification, const char* rtti_name, const std::string& name) :
  type(classification),
  name(name),
  short_name(RemoveNamespaces(name)),
  rtti_name(rtti_name),
  demangled_rtti_name(rrlib::util::Demangle(rtti_name)),
  size(0),
  generic_object_size(0),
  type_traits(0),
  new_info(true),
  uid(-1),
  element_type(NULL),
  list_type(NULL),
  shared_ptr_list_type(NULL),
  binary(),
  enum_strings(NULL),
  non_standard_enum_value_strings()
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

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
