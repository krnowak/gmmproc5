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

// standard
#include <algorithm>

// api
#include "function.h"
#include "param.h"

namespace Proc
{

namespace Api
{

struct Function::FunctionImpl
{
  FunctionImpl ();
  FunctionImpl (const std::string& id);

  std::string m_id;
  bool m_wrapped;
  std::string m_ret_type;
  Function::ParamList m_parameters;
  bool m_has_varargs;
};

Function::FunctionImpl::FunctionImpl ()
: m_id (),
  m_wrapped (false),
  m_ret_type (),
  m_parameters (),
  m_has_varargs (false)
{}

Function::FunctionImpl::FunctionImpl (const std::string& id)
: m_id (id),
  m_wrapped (false),
  m_ret_type (),
  m_parameters (),
  m_has_varargs (false)
{}

Function::Function ()
: FunctionBase (),
  m_pimpl (new FunctionImpl)
{}

Function::Function (const std::string& id)
: FunctionBase (),
  m_pimpl (new FunctionImpl (id))
{}

Function::~Function ()
{}

bool Function::get_has_varargs () const
{
  return m_pimpl->m_has_varargs;
}

void Function::set_has_varargs (bool has_varargs)
{
  m_pimpl->m_has_varargs = has_varargs;
}

void Function::swap (Function& function)
{
  m_pimpl.swap (function.m_pimpl);
}

void Function::set_ret_type_vfunc (const std::string& ret_type)
{
  m_pimpl->m_ret_type = ret_type;
}

std::string Function::get_ret_type_vfunc () const
{
  return m_pimpl->m_ret_type;
}

Function::ParamList::const_iterator Function::get_begin_vfunc () const
{
  return m_pimpl->m_parameters.begin ();
}

Function::ParamList::iterator Function::get_begin_vfunc ()
{
  return m_pimpl->m_parameters.begin ();
}

Function::ParamList::const_iterator Function::get_end_vfunc () const
{
  return m_pimpl->m_parameters.end ();
}

Function::ParamList::iterator Function::get_end_vfunc ()
{
  return m_pimpl->m_parameters.end ();
}

void Function::append_param_vfunc (const ParamPtr& param)
{
  m_pimpl->m_parameters.push_back (param);
}

} // namespace Api

} // namespace Proc
