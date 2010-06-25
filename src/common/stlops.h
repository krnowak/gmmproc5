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

#ifndef PROC_COMMON_STL_OPS_H
#define PROC_COMMON_STL_OPS_H

// standard
#include <utility>

namespace Proc
{

namespace Common
{

template <typename T>
struct PtrDeleter
{
  void operator() (T* ptr)
  {
    if (ptr)
    {
      delete ptr;
    }
  }
};

template <typename T1, typename T2>
struct PairDeleter
{
  void operator() (std::pair<T1*, const T2>& pair)
  {
    PtrDeleter<T1> () (pair.first);
  }
  
  void operator() (std::pair<const T1, T2*>& pair)
  {
    PtrDeleter<T2> () (pair.second);
  }
  
  void operator() (std::pair<T1*, T2*>& pair)
  {
    PtrDeleter<T1> () (pair.first);
    PtrDeleter<T2> () (pair.second);
  }
};

template <typename T>
struct PtrCopier
{
  T* operator() (T* ptr)
  {
    return new T (*ptr);
  }
};

} // namespace Common

} // namespace Proc

#endif // PROC_COMMON_STL_OPS_H
