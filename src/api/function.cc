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
// common
#include "apitemplates.h"
#include "stlops.h"
// api
#include "function.h"
#include "param.h"

namespace Proc
{

namespace Api
{

Function::Function (const std::string& id)
: Id (id),
  m_ret_val (),
  m_parameters (),
  m_has_varargs (false)
{}

Function::Function (const Function& function)
: Id (function),
  m_ret_val (function.m_ret_val),
  m_parameters (function.m_parameters.size ()),
  m_has_varargs (function.m_has_varargs)
{
  std::transform (function.m_parameters.begin(), function.m_parameters.end(), m_parameters.begin(), ::Proc::Common::PtrCopier<Param> ());
}

Function& Function::operator= (const Function& function)
{
  if (this != &function)
  {
    *static_cast<Id*>(this) = function;
    m_ret_val = function.m_ret_val;
    std::for_each (m_parameters.begin(), m_parameters.end(), ::Proc::Common::PtrDeleter<Param> ());
    m_parameters.resize (function.m_parameters.size ());
    std::transform (function.m_parameters.begin(), function.m_parameters.end(), m_parameters.begin(), ::Proc::Common::PtrCopier<Param> ());
  }
  return *this;
}

Function::~Function()
{
  std::for_each (m_parameters.begin(), m_parameters.end(), ::Proc::Common::PtrDeleter<Param> ());
}

bool Function::set_ret_val (const std::string& ret_val)
{
  return ::Proc::Common::FieldAssigner<std::string> () (m_ret_val, ret_val);
}

bool Function::append_param (Param* param)
{
  return ::Proc::Common::PlainInserter<Param, std::list<Param*> > () (m_parameters, param);
}

} // namespace Api

} // namespace Proc
