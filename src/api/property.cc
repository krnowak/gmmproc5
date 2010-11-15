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
#include "property.h"

namespace Proc
{

namespace Api
{

struct Property::PropertyImpl
{
  PropertyImpl ();
  PropertyImpl (const std::string& id);

  std::string m_id;
  bool m_wrapped;
  std::string m_type;
  ParamFlags m_param_flags;
};

Property::PropertyImpl::PropertyImpl ()
: m_id (),
  m_wrapped (false),
  m_type (),
  m_param_flags (PARAM_INVALID)
{}

Property::PropertyImpl::PropertyImpl (const std::string& id)
: m_id (id),
  m_wrapped (false),
  m_type (),
  m_param_flags (PARAM_INVALID)
{}

Property::Property ()
: Wrappable (),
  m_pimpl (new PropertyImpl)
{}

Property::Property (const std::string& id)
: Wrappable (),
  m_pimpl (new PropertyImpl (id))
{}

Property::~Property ()
{}

std::string Property::get_type () const
{
  return m_pimpl->m_type;
}

void Property::set_type (const std::string& type)
{
  m_pimpl->m_type = type;
}

ParamFlags Property::get_flags () const
{
  return m_pimpl->m_param_flags;
}

void Property::set_flags (ParamFlags param_flags)
{
  m_pimpl->m_param_flags = param_flags;
}

void Property::swap (Property& property)
{
  m_pimpl.swap (property.m_pimpl);
}

std::string Property::get_id_vfunc () const
{
  return m_pimpl->m_id;
}

void Property::set_id_vfunc (const std::string& id)
{
  m_pimpl->m_id = id;
}

bool Property::get_wrapped_vfunc () const
{
  return m_pimpl->m_wrapped;
}

void Property::set_wrapped_vfunc (bool wrapped)
{
  m_pimpl->m_wrapped = wrapped;
}

} // namespace Api

} // namespace Proc
