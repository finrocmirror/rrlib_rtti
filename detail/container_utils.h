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
/*!\file    rrlib/rtti/detail/container_utils.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-02-05
 *
 * \brief
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__rtti__detail__container_utils_h__
#define __rrlib__rtti__detail__container_utils_h__

/*!
 * This file contains some utility functions for STL containers needed in this rrlib
 */

namespace rrlib
{
namespace rtti
{
namespace detail
{

/*!
 * Helper to be able to resize vectors of noncopyable types
 */
template <typename C, typename E, bool COPYABLE>
struct tResize
{
  static inline void Resize(C& c, size_t new_size)
  {
    c.resize(new_size);
  }
};

template <typename C, typename E>
struct tResize<C, E, false>
{
  static inline void Resize(C& c, size_t new_size)
  {
    if (c.size() < new_size)
    {
      c.reserve(new_size);
    }
    while (c.size() < new_size)
    {
      c.push_back(sStaticTypeInfo<E>::CreateByValue());
    }
    while (c.size() > new_size)
    {
      c.pop_back();
    }
  }
};

template <typename C, bool b>
struct tSimpleContainerCopy
{
  static inline void Copy(const C& src, C& dest)
  {
    dest = src;
  }
};

template <typename C>
struct tSimpleContainerCopy<C, true>
{
  static inline void Copy(const C& src, C& dest)
  {
    assert(false && "Programming error below (?)");
  }
};


template <typename C, typename T>
void CopySTLContainer(const C& src, C& dest, rtti::tFactory* f)
{
  if (std::is_base_of<boost::noncopyable, T>::value)
  {
    detail::tResize < C, T, !std::is_base_of<boost::noncopyable, T>::value >::Resize(dest, src.size());

    for (size_t i = 0; i < src.size(); i++)
    {
      DeepCopyImpl(src[i], dest[i], f);
    }
  }
  else
  {
    tSimpleContainerCopy<C, std::is_base_of<boost::noncopyable, T>::value>::Copy(src, dest);
  }
}

template <typename C, typename T>
void CopySmartPointerSTLContainer(const C& src, C& dest, rtti::tFactory* f)
{
  dest.resize(src.size());

  tType type_t = tDataType<T>();
  tDefaultFactory df;
  if (!f)
  {
    f = &df;
  }

  for (size_t i = 0; i < src.size(); i++)
  {
    if (!src[i])
    {
      dest[i].reset();
      continue;
    }
    tType needed = tDataType<T>::FindTypeByRtti(typeid(*src[i]).name());
    if ((!dest[i]) || tDataType<T>::FindTypeByRtti(typeid(*dest[i]).name()) != needed)
    {
      f->CreateBuffer(dest[i], needed);
    }
    if (needed != type_t)
    {
      needed.DeepCopy(src[i].get(), dest[i].get(), f);
    }
    else
    {
      sStaticTypeInfo<T>::DeepCopy(*src[i], *dest[i], f);
    }
  }
}

} // namespace
} // namespace
} // namespace

#endif // __rrlib__rtti__detail__container_utils_h__
