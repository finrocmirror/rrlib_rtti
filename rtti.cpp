/**
 * You received this file as part of RRLib serialization
 *
 * Copyright (C) 2012 Max Reichardt,
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
#include <cxxabi.h>
#include <execinfo.h>

namespace rrlib
{
namespace rtti
{

std::string Demangle(const char* mangled)
{
  int status = 0;
  char* tmp = abi::__cxa_demangle(mangled, 0, 0, &status);
  std::string result(tmp);
  free(tmp);
  return result;
}

static std::string GetSoFile(const std::string& backtrace_entry)
{
  return backtrace_entry.substr(0, backtrace_entry.find('('));
}

std::string GetBinaryCurrentlyPerformingStaticInitialization()
{
  // system .so file that does dynamic loading
  static std::string ld_so_file;

  // implementation uses backtrace to find this out
  void* array[255];
  int len = backtrace(array, 255);
  if (len == 0)
  {
    RRLIB_LOG_PRINT_STATIC(ERROR, "Empty stack trace.");
    return "";
  }
  char** symbols = backtrace_symbols(array, len);
  if (ld_so_file.length() == 0)
  {
    ld_so_file = GetSoFile(symbols[len - 1]);
    RRLIB_LOG_PRINT_STATIC(DEBUG_VERBOSE_1, "System library for loading .so files: ", ld_so_file);
  }

  for (int i = 1; i < len; i++)
  {
    std::string so = GetSoFile(symbols[i]);
    if (so.compare(ld_so_file) == 0)
    {
      std::string result = GetSoFile(symbols[i - 1]);
      free(symbols);
      return result;
    }
  }

  free(symbols);
  return "";
}


} // namespace
} // namespace

