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

// api
#include "param.h"

namespace Proc
{

namespace Api
{

struct Param::ParamImpl
{
  ParamImpl ();
  ParamImpl (const std::string& id);

  std::string m_id;
  std::string m_type;
};

Param::ParamImpl::ParamImpl ()
: m_id (),
  m_type ()
{}

Param::ParamImpl::ParamImpl (const std::string& id)
: m_id (id),
  m_type ()
{}

Param::Param ()
: Identifiable (),
  m_pimpl (new ParamImpl)
{}

Param::Param (const std::string& id)
: Identifiable (),
  m_pimpl (new ParamImpl (id))
{}

Param::~Param ()
{}

void Param::set_type (const std::string& type)
{
  m_pimpl->m_type = type;
}

std::string Param::get_type () const
{
  return m_pimpl->m_type;
}

void Param::swap (Param& param)
{
  m_pimpl.swap (param.m_pimpl);
}

std::string Param::get_id_vfunc () const
{
  return m_pimpl->m_id;
}

void Param::set_id_vfunc (const std::string& id)
{
  m_pimpl->m_id = id;
}

} // namespace Api

} // namespace Proc
