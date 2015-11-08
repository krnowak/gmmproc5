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
#include "identifiable.h"

namespace Proc
{

namespace Api
{

Identifiable::~Identifiable ()
{}

std::string Identifiable::get_id () const
{
  return get_id_vfunc ();
}

void Identifiable::set_id (const std::string& id)
{
  set_id_vfunc (id);
}

bool operator== (const Identifiable& lhs, const Identifiable& rhs)
{
  return (lhs.get_id () == rhs.get_id ());
}

bool operator!= (const Identifiable& lhs, const Identifiable& rhs)
{
  return (lhs.get_id () != rhs.get_id ());
}

bool operator< (const Identifiable& lhs, const Identifiable& rhs)
{
  return (lhs.get_id () < rhs.get_id ());
}

bool operator<= (const Identifiable& lhs, const Identifiable& rhs)
{
  return (lhs.get_id () <= rhs.get_id ());
}

bool operator> (const Identifiable& lhs, const Identifiable& rhs)
{
  return (lhs.get_id () > rhs.get_id ());
}

bool operator>= (const Identifiable& lhs, const Identifiable& rhs)
{
  return (lhs.get_id () >= rhs.get_id ());
}

} // namespace Api

} // namespace Proc
