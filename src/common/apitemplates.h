/* gmmproc5
 *
 * Copyright (C) 2010 Krzesimir Nowak
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef PROC_COMMON_API_TEMPLATES_H
#define PROC_COMMON_API_TEMPLATES_H

// standard
#include <map>
#include <string>
#include <utility>

namespace Proc
{

namespace Common
{

template <typename T>
struct IdInserter
{
  bool operator() (std::map<std::string, T*>& attrs, T* attr)
  {
    bool ok (false);
    
    if (attr)
    {
      std::string name (attr->get_id ());
      
      if (!name.empty ())
      {
        ok = attrs.insert (std::make_pair (name, attr)).second;
      }
    }
    return ok;
  }
};

template <typename T, typename Container>
struct PlainInserter
{
  bool operator() (Container& container, T* ptr)
  {
    bool ok (false);
    
    if (ptr)
    {
      container.push_back (ptr);
      ok = true;
    }
    return ok;
  }
};

template <typename T>
struct ZeroTraits
{
  typedef T Type;
  static const Type zero;
};

template <typename T>
const typename ZeroTraits<T>::Type ZeroTraits<T>::zero = 0;

template <>
struct ZeroTraits<std::string>
{
  typedef std::string Type;
  static const Type zero;
};

template <typename T, typename Tr = ZeroTraits<T> >
struct FieldAssigner
{
  bool operator() (T& field, const T& value)
  {
    bool ok (false);
    
    if ((field == Tr::zero) && (value != Tr::zero))
    {
      field = value;
      ok = true;
    }
    return ok;
  }
};

} // namespace Common

} // namespace Proc

#endif // PROC_COMMON_API_TEMPLATES_H
