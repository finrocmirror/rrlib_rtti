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
/*!\file    rrlib/rtti/detail/sStaticTypeInfoDefaultImpl.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__sStaticTypeInfoDefaultImpl_h__
#define __rrlib__rtti__detail__sStaticTypeInfoDefaultImpl_h__

#include <vector>
#include <list>
#include <deque>
#include "rrlib/logging/messages.h"
#include "rrlib/rtti/detail/deep_copy.h"

namespace rrlib
{
namespace rtti
{
namespace detail
{

template <typename C, typename T>
void CopySTLContainer(const C& src, C& dest, rtti::tFactory* f);

template <typename C, typename T>
void CopySmartPointerSTLContainer(const C& src, C& dest, rtti::tFactory* f);

/*!
 * \author Max Reichardt
 *
 * Default implementations for tStaticTypeInfo.
 */

template <typename C, typename T>
struct tStaticTypeInfoSTLContainer
{
  static const bool stl_container_suitable = false; // we currently do not support vectors of vectors - it should be possible though
  static const bool shared_ptr_stl_container_suitable = false; // dito
  static const bool valid_equal_to_operator = tHasEqualToOperator<T>::value; // only use == if value type supports it

  static void Clear(C& c) {}

  static C* Create(void* placement)
  {
    return new(placement) C();
  }

  static C CreateByValue()
  {
    return C();
  }

  static void DeepCopy(const C& src, C& dest, tFactory* f = NULL)
  {
    CopySTLContainer<C, T>(src, dest, f);
  }
};

template <typename C, typename T>
struct tStaticTypeInfoSmartPtrSTLContainer
{
  static const bool stl_container_suitable = false; // we currently do not support vectors of vectors - it should be possible though
  static const bool shared_ptr_stl_container_suitable = false; // dito
  static const bool valid_equal_to_operator = tHasEqualToOperator<T>::value; // only use == if value type supports it

  static void Clear(C& c)
  {
    c.clear();
  }

  static C* Create(void* placement)
  {
    return new(placement) C();
  }

  static C CreateByValue()
  {
    return C();
  }

  static void DeepCopy(const C& src, C& dest, tFactory* f = NULL)
  {
    CopySmartPointerSTLContainer<C, T>(src, dest, f);
  }
};

template <typename T, bool abstract>
struct sStaticTypeInfoDefaultImplBase
{
  static const bool stl_container_suitable = !(std::is_same<bool, T>::value || std::is_base_of<tIsListType<false, false>, T>::value || std::is_base_of<tIsListType<false, true>, T>::value);
  //static const bool shared_ptr_stl_container_suitable = !(std::has_trivial_destructor<T>::value || std::is_base_of<tIsListType<false, false>, T>::value || std::is_base_of<tIsListType<true, false>, T>::value);
  static const bool shared_ptr_stl_container_suitable = std::is_abstract<T>::value || std::is_base_of<tIsListType<false, true>, T>::value || std::is_base_of<tIsListType<true, true>, T>::value;
  static const bool valid_equal_to_operator = tHasEqualToOperator<T>::value;

  static void Clear(T& t) {}

  /*!
   * Creates object at specified address. Used by tDataType.
   */
  static T* Create(void* placement)
  {
    return new(placement) T();
  }

  /*!
   * Creates object by value. Used when adding elements to STL containers.
   */
  static T CreateByValue()
  {
    return T();
  }

  static void DeepCopy(const T& src, T& dest, tFactory* f = NULL)
  {
    DeepCopyImpl(src, dest, f);
  }
};

template <typename T>
struct sStaticTypeInfoDefaultImplBase<T, true>
{
  static const bool stl_container_suitable = false;
  static const bool shared_ptr_stl_container_suitable = false;
  static const bool valid_equal_to_operator = tHasEqualToOperator<T>::value;

  static void Clear(T& t) {}

  /*!
   * Creates object at specified address. Used by tDataType.
   */
  static T* Create(void* placement)
  {
    RRLIB_LOG_PRINT(ERROR, "Abstract types cannot be created");
    return NULL;
  }

  /*!
   * Creates object by value. Used when adding elements to STL containers.
   */
  static T& CreateByValue()
  {
    RRLIB_LOG_PRINT(ERROR, "Abstract types cannot be created");
    return *((T*)NULL);
  }

  static void DeepCopy(const T& src, T& dest, tFactory* f = NULL)
  {
    DeepCopyImpl(src, dest, f);
  }
};

template <typename T>
struct sStaticTypeInfoDefaultImpl : public sStaticTypeInfoDefaultImplBase<T, std::is_abstract<T>::value> {};

template <typename T>
struct sStaticTypeInfoDefaultImpl<std::vector<T>> : public tStaticTypeInfoSTLContainer<std::vector<T>, T> {};

template <typename T>
struct sStaticTypeInfoDefaultImpl<std::list<T>> : public tStaticTypeInfoSTLContainer<std::list<T>, T> {};

template <typename T>
struct sStaticTypeInfoDefaultImpl<std::deque<T>> : public tStaticTypeInfoSTLContainer<std::deque<T>, T> {};

template <typename T>
struct sStaticTypeInfoDefaultImpl<std::vector<std::shared_ptr<T>>> : public tStaticTypeInfoSmartPtrSTLContainer<std::vector<std::shared_ptr<T>>, T> {};

template <typename T>
struct sStaticTypeInfoDefaultImpl<std::list<std::shared_ptr<T>>> : public tStaticTypeInfoSmartPtrSTLContainer<std::list<std::shared_ptr<T>>, T> {};

template <typename T>
struct sStaticTypeInfoDefaultImpl<std::deque<std::shared_ptr<T>>> : public tStaticTypeInfoSmartPtrSTLContainer<std::deque<std::shared_ptr<T>>, T> {};


} // namespace
} // namespace
} // namespace

#endif // __rrlib__rtti__detail__sStaticTypeInfoDefaultImpl_h__
