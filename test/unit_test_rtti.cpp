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
/*!\file    rrlib/rtti/test/unit_test_rtti.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2014-03-27
 *
 * Tests type traits, generic operations and tDataType for different types
 */
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tUnitTestSuite.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"

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
namespace test
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
static_assert(detail::HasCopyFromMethod<serialization::tMemoryBuffer>::value, "Trait not implemented correctly");

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

class Class1 {};
class Class2 {};
class RenamedClass {};


class tTestTraitsRtti : public rrlib::util::tUnitTestSuite
{
  RRLIB_UNIT_TESTS_BEGIN_SUITE(tTestTraitsRtti);
  RRLIB_UNIT_TESTS_ADD_TEST(TestTypeNaming);
  RRLIB_UNIT_TESTS_ADD_TEST(TestGenericOperations);
  RRLIB_UNIT_TESTS_END_SUITE;

private:

  virtual void InitializeTests() override
  {

  }

  virtual void CleanUp() override {}

  void TestTypeNaming()
  {
    {
      tDataType<Class1> type;
      tDataType<std::vector<Class1>> type_list;
      RRLIB_UNIT_TESTS_EQUALITY_MESSAGE("Names '" + type.GetName() + "' and 'rrlib.rtti.test.Class1' are not equal", type.GetName() == "rrlib.rtti.test.Class1", true);
      RRLIB_UNIT_TESTS_EQUALITY_MESSAGE("Names '" + type_list.GetName() + "' and 'List<rrlib.rtti.test.Class1>' are not equal", type_list.GetName() == "List<rrlib.rtti.test.Class1>", true);
    }
    {
      tDataType<std::vector<Class2>> type_list;
      tDataType<Class2> type;
      RRLIB_UNIT_TESTS_EQUALITY_MESSAGE("Names '" + type.GetName() + "' and 'rrlib.rtti.test.Class2' are not equal", type.GetName() == "rrlib.rtti.test.Class2", true);
      RRLIB_UNIT_TESTS_EQUALITY_MESSAGE("Names '" + type_list.GetName() + "' and 'List<rrlib.rtti.test.Class2>' are not equal", type_list.GetName() == "List<rrlib.rtti.test.Class2>", true);
    }
    {
      tDataType<RenamedClass> type("Class3");
      tDataType<std::vector<RenamedClass>> type_list;
      RRLIB_UNIT_TESTS_EQUALITY_MESSAGE("Names '" + type.GetName() + "' and 'Class3' are not equal", type.GetName() == "Class3", true);
      RRLIB_UNIT_TESTS_EQUALITY_MESSAGE("Names '" + type_list.GetName() + "' and 'List<Class3>' are not equal", type_list.GetName() == "List<Class3>", true);
    }
  }

  template <typename T>
  void TestGenericOperations(T& t)
  {
    tGenericObjectWrapper<T> wrapper(t);
    tGenericObject* copy = wrapper.GetType().CreateInstanceGeneric();
    copy->DeepCopyFrom(wrapper);
    RRLIB_UNIT_TESTS_EQUALITY_MESSAGE("Objects must be equal", copy->Equals(wrapper), true);
  }

  void TestGenericOperations()
  {
    typedef serialization::tMemoryBuffer tBuffer;
    int i = 3;
    TestGenericOperations<int>(i);
    std::string test_string = " test\nstring ";
    TestGenericOperations<std::string>(test_string);
    std::vector<int> test_vector = { 7, 8, 9 };
    TestGenericOperations<std::vector<int>>(test_vector);
    tBuffer buffer;
    serialization::tOutputStream stream(buffer);
    for (int i = 0; i < 20000; i++)
    {
      stream.WriteInt(i);
    }
    stream.Close();
    TestGenericOperations<tBuffer>(buffer);
    std::map<size_t, tBuffer> m;
    m[1] = std::move(buffer);
    m[3] = tBuffer();
    TestGenericOperations<std::map<size_t, tBuffer>>(m);
  }
};

RRLIB_UNIT_TESTS_REGISTER_SUITE(tTestTraitsRtti);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
