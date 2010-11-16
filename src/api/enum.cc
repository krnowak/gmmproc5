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
#include "enum.h"

namespace Proc
{

namespace Api
{

struct Enum::EnumImpl
{
  EnumImpl ();
  EnumImpl (const std::string& id);

  std::string m_id;
  bool m_wrapped;
  Enum::ElementList m_elements;
  bool m_is_flags;
};

Enum::EnumImpl::EnumImpl ()
: m_id (),
  m_wrapped (false),
  m_elements (),
  m_is_flags (false)
{}

Enum::EnumImpl::EnumImpl (const std::string& id)
: m_id (id),
  m_wrapped (false),
  m_elements (),
  m_is_flags (false)
{}

Enum::Enum ()
: Wrappable (),
  m_pimpl (new EnumImpl ())
{}

Enum::Enum (const std::string& id)
: Wrappable (),
  m_pimpl (new EnumImpl (id))
{}

Enum::~Enum ()
{}

void Enum::append_element (const ElementPtr& element)
{
  m_pimpl->m_elements.push_back (element);
}

bool Enum::get_is_flags () const
{
  return m_pimpl->m_is_flags;
}

void Enum::set_is_flags (bool is_flags)
{
  m_pimpl->m_is_flags = is_flags;
}

Enum::ElementList::const_iterator Enum::get_begin () const
{
  return const_cast<Enum*> (this)->get_begin ();
}

Enum::ElementList::iterator Enum::get_begin ()
{
  return m_pimpl->m_elements.begin ();
}

Enum::ElementList::const_iterator Enum::get_end () const
{
  return const_cast<Enum*> (this)->get_end ();
}

Enum::ElementList::iterator Enum::get_end ()
{
  return m_pimpl->m_elements.end ();
}

void Enum::swap (Enum& enumeration)
{
  m_pimpl.swap (enumeration.m_pimpl);
}

std::string Enum::get_id_vfunc () const
{
  return m_pimpl->m_id;
}

void Enum::set_id_vfunc (const std::string& id)
{
  m_pimpl->m_id = id;
}

bool Enum::get_wrapped_vfunc () const
{
  return m_pimpl->m_wrapped;
}

void Enum::set_wrapped_vfunc (bool wrapped)
{
  m_pimpl->m_wrapped = wrapped;
}

} // namespace Api

} // namespace Proc
