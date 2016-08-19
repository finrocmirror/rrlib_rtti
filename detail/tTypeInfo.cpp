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
/*!\file    rrlib/rtti/detail/tTypeInfo.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2016-07-20
 *
 */
//----------------------------------------------------------------------
#include "rrlib/rtti/detail/tTypeInfo.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/demangle.h"
#include "rrlib/util/string.h"
#include <unordered_map>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/tType.h"

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
namespace detail
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

/*! we need to avoid reallocation in order to make vector thread safe. Therefore it is created with this capacity. */
const size_t cMAX_TYPES = 1000;

static tTypeInfo::tSharedInfo cNULL_TYPE_SHARED_INFO(nullptr);
const tTypeInfo tTypeInfo::cNULL_TYPE_INFO = { typeid(std::nullptr_t), trait_flags::cSUPPORTS_BITWISE_COPY | trait_flags::cIS_DEFAULT_CONSTRUCTION_ZERO_MEMORY, &cNULL_TYPE_SHARED_INFO, 0 };

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

struct tInternalData
{
  std::recursive_mutex mutex;
  std::vector<const tTypeInfo*> types;
  size_t next_annotation_index;
  typedef std::pair<util::tManagedConstCharPointer, util::tManagedConstCharPointer> tRenamingEntry;
  std::vector<tRenamingEntry> renamed_types;
  typedef std::pair<const char*, unsigned int> tAnnotationTableEntry; // typeid(TAnnotation).name(), offset
  std::vector<tAnnotationTableEntry> annotation_table;
  typedef std::pair<const char*, const tTypeInfo*> tNameLookupEntry; // name, type info
  std::vector<tNameLookupEntry> name_lookup;

  tInternalData() : mutex(), types(), next_annotation_index(0)
  {
    types.reserve(cMAX_TYPES); // we need to avoid reallocation in order to make vector thread safe
  }
};

/*!
 * Comparator functor for desired order in tInternalData::renamed_types
 */
struct tRenamedTypeCompare
{
  bool operator()(const tInternalData::tRenamingEntry& lhs, const tInternalData::tRenamingEntry& rhs) const
  {
    size_t len_lhs = strlen(lhs.first.Get());
    size_t len_rhs = strlen(rhs.first.Get());
    if (len_lhs != len_rhs)
    {
      return len_lhs > len_rhs;
    }
    return strcmp(lhs.first.Get(), rhs.first.Get()) < 0;
  }
};

static tInternalData& GetInternalData()
{
  static tInternalData internal_data;
  return internal_data;
}

const std::vector<const tTypeInfo*>* tTypeInfo::tSharedInfo::registered_types = &GetInternalData().types;

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

const tType tTypeInfo::FindType(const std::string& name)
{
  if (name == "NULL")
  {
    return tType();
  }
  bool list_type = util::StartsWith(name, "List<") && util::EndsWith(name, ">");
  if (list_type)
  {
    return tType::FindType(name.substr(5, name.length() - 6)).GetListType();
  }

  // main names
  static tInternalData& internal_data = GetInternalData();
  for (const tTypeInfo * info : internal_data.types)
  {
    if (name == info->shared_info->name.Get())
    {
      return tType(info);
    }
  }

  // additional lookup
  {
    std::unique_lock<std::recursive_mutex> lock(internal_data.mutex);
    tInternalData::tNameLookupEntry entry(name.c_str(), nullptr);
    auto it = std::lower_bound(internal_data.name_lookup.begin(), internal_data.name_lookup.end(), entry);
    if (it != internal_data.name_lookup.end() && it->first == name)
    {
      return tType(it->second);
    }
  }

  // Try omitting one namespace
  if (name.find('.') != std::string::npos) // namespaces in specified name
  {
    tType type = FindType(tSharedInfo::RemoveNamespaces(name));
    if (type != nullptr)
    {
      return type;
    }
  }
  else // no namespace in specified name
  {
    for (const tTypeInfo * info : internal_data.types)
    {
      char buffer[name.length() + 2];
      buffer[0] = '.';
      memcpy(&buffer[1], name.c_str(), name.length());
      buffer[name.length() + 1] = 0;
      if (rrlib::util::EndsWith(info->shared_info->name.Get(), buffer))
      {
        return tType(info);
      }
    }
  }

  return tType();
}

const tType tTypeInfo::FindTypeByRtti(const char* rtti_name)
{
  static tInternalData& internal_data = GetInternalData();
  for (const tTypeInfo * info : internal_data.types)
  {
    if (info->std_type_info.name() == rtti_name)
    {
      return tType(info);
    }
  }
  return tType();
}

util::tManagedConstCharPointer tTypeInfo::GetTypeNameDefinedInRRLibRtti(const tType& type)
{
  static const char* cTABLE[] =
  {
    typeid(int8_t).name(), "int8",
    typeid(int16_t).name(), "int16",
    typeid(int).name(), "int32",
    typeid(long long).name(), "int64",
    typeid(uint8_t).name(), "uint8",
    typeid(uint16_t).name(), "uint16",
    typeid(unsigned int).name(), "uint32",
    typeid(unsigned long long).name(), "uint64",
    typeid(std::string).name(), "String",
    typeid(double).name(), "double",
    typeid(float).name(), "float",
    typeid(bool).name(), "bool",
    typeid(rrlib::time::tDuration).name(), "Duration",
    typeid(rrlib::time::tTimestamp).name(), "Timestamp"
  };

  for (size_t i = 0; i < sizeof(cTABLE) / sizeof(const char*); i += 2)
  {
    if (type.GetRttiName() == cTABLE[i])
    {
      return util::tManagedConstCharPointer(cTABLE[i + 1], false);
    }
  }
  throw std::runtime_error(std::string("Could not find type for ") + type.GetRttiName());
}

util::tManagedConstCharPointer tTypeInfo::GetDefaultTypeName(const tType& type)
{
  std::string demangled = util::Demangle(type.GetRttiName());

  // Do we have a template? => check for string replacements
  if (demangled.find('<') != std::string::npos)
  {
    ReplaceContainerNames(demangled);
    static tInternalData& internal_data = GetInternalData();
    std::unique_lock<std::recursive_mutex> lock(internal_data.mutex);

    // TODO: std::regex might be more convenient (available on all relevant platforms?)
    for (auto & entry : internal_data.renamed_types)
    {
      size_t found_index = 0;

      while ((found_index = demangled.find(entry.first.Get(), found_index + 1)) != std::string::npos)
      {
        // we only want to replace whole words => character before and after must not be a letter
        int demangled_len = strlen(entry.first.Get());
        if ((!isalnum(demangled[found_index - 1])) && found_index + demangled_len < demangled.length() &&
            (!isalnum(demangled[found_index + demangled_len])))
        {
          demangled.replace(found_index, demangled_len, entry.second.Get());
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

  return util::tManagedConstCharPointer(name, true);
}

tTypeInfo::tSharedInfo::tSharedInfo(const tTypeInfo* type_info, const tTypeInfo* type_info_list, const tTypeInfo* underlying_type, tGetTypenameFunction get_typename_function, bool register_types_now) :
  tSharedInfo(type_info, type_info_list, underlying_type, (*get_typename_function)(tType(type_info)), get_typename_function != &GetDefaultTypeName, register_types_now)
{}

tTypeInfo::tSharedInfo::tSharedInfo(const tTypeInfo* type_info, const tTypeInfo* type_info_list, const tTypeInfo* underlying_type, util::tManagedConstCharPointer name, bool register_types_now) :
  tSharedInfo(type_info, type_info_list, underlying_type, std::move(name), true, register_types_now)
{}

tTypeInfo::tSharedInfo::tSharedInfo(const tTypeInfo* type_info, const tTypeInfo* type_info_list, const tTypeInfo* underlying_type, const char* name, bool register_types_now) :
  tSharedInfo(type_info, type_info_list, underlying_type, util::tManagedConstCharPointer(name, false), true, register_types_now)
{}

tTypeInfo::tSharedInfo::tSharedInfo(std::nullptr_t) :
  name("NULL", false),
  underlying_type(&cNULL_TYPE_INFO)
{
  memset(annotations, 0, sizeof(annotations));
  uid[0] = -1;
  uid[1] = -1;
}

tTypeInfo::tSharedInfo::tSharedInfo(const tTypeInfo* type_info, const tTypeInfo* type_info_list, const tTypeInfo* underlying_type, util::tManagedConstCharPointer name, bool non_standard_name, bool register_types_now) :
  name(),
  underlying_type(underlying_type)
{
  memset(annotations, 0, sizeof(annotations));
  memset(uid, 0, sizeof(uid));
  this->name = std::move(name);

  static tInternalData& internal_data = GetInternalData();
  std::unique_lock<std::recursive_mutex> lock(internal_data.mutex);
  if (non_standard_name && (type_info->type_traits & trait_flags::cIS_DATA_TYPE))
  {
    tInternalData::tRenamingEntry entry(util::tManagedConstCharPointer(util::Demangle(type_info->std_type_info.name()).c_str(), true), util::tManagedConstCharPointer(this->name.Get(), false));

    // Search for existing entry
    bool found = false;
    for (auto & e : internal_data.renamed_types)
    {
      if (strcmp(e.first.Get(), entry.first.Get()) == 0)
      {
        // Custom entry has already been inserted
        this->name = util::tManagedConstCharPointer(e.second.Get(), false);
        found = true;
        break;
      }
    }
    if (!found)
    {
      auto insert_it = std::upper_bound(internal_data.renamed_types.begin(), internal_data.renamed_types.end(), entry, tRenamedTypeCompare());
      internal_data.renamed_types.insert(insert_it, std::move(entry));
    }
  }

  if (register_types_now)
  {
    Register(type_info, type_info_list);
  }
}

void tTypeInfo::tSharedInfo::AddAnnotationImplementation(const void* annotation, size_t size, const char* rtti_name, tAnnotationIndex& annotation_index)
{
  tInternalData& internal_data = GetInternalData();
  std::unique_lock<std::recursive_mutex> lock(internal_data.mutex);

  tInternalData::tAnnotationTableEntry entry(nullptr, 0);
  for (auto & e : internal_data.annotation_table)
  {
    if (e.first == rtti_name)
    {
      entry = e;
      break;
    }
  }
  if (!entry.first)
  {
    annotation_index.first = internal_data.next_annotation_index;
    annotation_index.second = true;
    internal_data.next_annotation_index += size;
    if (internal_data.next_annotation_index > cMAX_ANNOTATION_SIZE)
    {
      RRLIB_LOG_PRINT(ERROR, "Maximum number of annotations (tType::cMAX_ANNOTATION_SIZE) exceeded. Aborting.");
      abort();
    }
    internal_data.annotation_table.emplace_back(rtti_name, annotation_index.first);
  }
  size_t ann_index = annotation_index.first;
  assert(ann_index + size < cMAX_ANNOTATION_SIZE);
  memcpy(&annotations[ann_index], annotation, size);
}

void tTypeInfo::tSharedInfo::AddName(const tTypeInfo* type_info, const char* name)
{
  tInternalData& internal_data = GetInternalData();
  std::unique_lock<std::recursive_mutex> lock(internal_data.mutex);
  tInternalData::tNameLookupEntry entry(name, type_info);
  auto insert_it = std::upper_bound(internal_data.name_lookup.begin(), internal_data.name_lookup.end(), entry);
  internal_data.name_lookup.insert(insert_it, entry);
}

void tTypeInfo::tSharedInfo::Register(const tTypeInfo* type_info, const tTypeInfo* type_info_list)
{
  static tInternalData& internal_data = GetInternalData();
  std::unique_lock<std::recursive_mutex> lock(internal_data.mutex);

  size_t new_types = type_info_list ? 2 : 1;
  if (internal_data.types.size() + new_types > cMAX_TYPES)
  {
    RRLIB_LOG_PRINT(ERROR, "Maximum number of data types exceeded. Increase cMAX_TYPES.");
    throw std::runtime_error("Maximum number of data types exceeded. Increase cMAX_TYPES.");
  }

  uid[0] = internal_data.types.size();
  internal_data.types.push_back(type_info);
  if (type_info_list)
  {
    uid[1] = internal_data.types.size();
    internal_data.types.push_back(type_info_list);
  }
  RRLIB_LOG_PRINT(DEBUG_VERBOSE_1, "Adding data type ", name.Get());
}

std::string tTypeInfo::tSharedInfo::RemoveNamespaces(const std::string& type_name)
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

void tTypeInfo::tSharedInfo::SetName(util::tManagedConstCharPointer new_name, const tTypeInfo* type_info)
{
  static tInternalData& internal_data = GetInternalData();
  std::unique_lock<std::recursive_mutex> lock(internal_data.mutex);
  name = std::move(new_name);
  RRLIB_LOG_PRINT(DEBUG, type_info->std_type_info.name(), " " , this->name.Get());

  tInternalData::tRenamingEntry entry(util::tManagedConstCharPointer(util::Demangle(type_info->std_type_info.name()).c_str(), true), util::tManagedConstCharPointer(name.Get(), false));

  // Search for existing entry
  for (auto & e : internal_data.renamed_types)
  {
    if (strcmp(e.first.Get(), entry.first.Get()) == 0)
    {
      // Custom entry has already been inserted
      abort();
    }
  }
  auto insert_it = std::upper_bound(internal_data.renamed_types.begin(), internal_data.renamed_types.end(), entry, tRenamedTypeCompare());
  internal_data.renamed_types.insert(insert_it, std::move(entry));
}

tTypeInfo::tSharedInfoEnum::tSharedInfoEnum(const tTypeInfo* type_info, const tTypeInfo* type_info_list, const tTypeInfo* underlying_type, tGetTypenameFunction get_typename_function, make_builder::internal::tEnumStrings& enum_strings) :
  tSharedInfo(type_info, type_info_list, underlying_type, get_typename_function, false),
  enum_strings(enum_strings)
{
  // register types now
  this->Register(type_info, type_info_list);
}



//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
