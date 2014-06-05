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
/*!\file    rrlib/rtti/tests/rtti.cpp
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


class tTestTraitsRtti : public util::tUnitTestSuite
{
  RRLIB_UNIT_TESTS_BEGIN_SUITE(tTestTraitsRtti);
  RRLIB_UNIT_TESTS_ADD_TEST(TestTypeNaming);
  RRLIB_UNIT_TESTS_ADD_TEST(TestGenericOperations);
  RRLIB_UNIT_TESTS_END_SUITE;

private:

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
    static_assert(rrlib::serialization::IsBinarySerializable<T>::value, "Trait not correctly implemented");
    tGenericObjectWrapper<T> wrapper(t);
    tGenericObject* copy = wrapper.GetType().CreateInstanceGeneric();
    RRLIB_UNIT_TESTS_EQUALITY_MESSAGE(std::string("Objects must not be equal (type: ") + util::Demangle(typeid(T).name()) + ")", copy->Equals(wrapper), false);
    copy->DeepCopyFrom(wrapper);
    RRLIB_UNIT_TESTS_EQUALITY_MESSAGE(std::string("Objects must be equal (type: ") + util::Demangle(typeid(T).name()) + ")", copy->Equals(wrapper), true);
  }

  void TestGenericOperations()
  {
    typedef serialization::tMemoryBuffer tBuffer;
    int i = 3;
    TestGenericOperations(i);
    std::string test_string = " test\nstring ";
    TestGenericOperations(test_string);
    std::vector<int> test_vector = { 7, 8, 9 };
    TestGenericOperations(test_vector);
    std::vector<bool> test_vector_bool = { false, true, true };
    TestGenericOperations(test_vector_bool);
    std::vector<std::vector<double>> test_double_vector = { { 3, 4 }, { 2 } };
    TestGenericOperations(test_double_vector);

    tBuffer buffer;
    serialization::tOutputStream stream(buffer);
    for (int i = 0; i < 20000; i++)
    {
      stream.WriteInt(i);
    }
    stream.Close();
    TestGenericOperations(buffer);
    std::map<size_t, tBuffer> m;
    m[1] = std::move(buffer);
    m[3] = tBuffer();
    TestGenericOperations(m);
  }
};

RRLIB_UNIT_TESTS_REGISTER_SUITE(tTestTraitsRtti);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
