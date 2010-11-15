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

#include "functionbase.h"

namespace Proc
{

namespace Api
{

FunctionBase::~FunctionBase ()
{}

std::string FunctionBase::get_ret_type () const
{
  return get_ret_type_vfunc ();
}

void FunctionBase::set_ret_type (const std::string& ret_type)
{
  set_ret_type_vfunc (ret_type);
}

FunctionBase::ParamList::const_iterator FunctionBase::get_begin () const
{
  return get_begin_vfunc ();
}

FunctionBase::ParamList::iterator FunctionBase::get_begin ()
{
  return get_begin_vfunc ();
}

FunctionBase::ParamList::const_iterator FunctionBase::get_end () const
{
  return get_end_vfunc ();
}

FunctionBase::ParamList::iterator FunctionBase::get_end ()
{
  return get_end_vfunc ();
}

void FunctionBase::append_param (const FunctionBase::ParamPtr& param)
{
  append_param_vfunc (param);
}

} // namespace Api

} // namespace Proc
