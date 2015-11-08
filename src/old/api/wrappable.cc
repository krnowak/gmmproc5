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
#include "wrappable.h"

namespace Proc
{

namespace Api
{

Wrappable::~Wrappable ()
{}

bool Wrappable::get_wrapped () const
{
  return get_wrapped_vfunc ();
}

void Wrappable::set_wrapped (bool wrapped)
{
  set_wrapped_vfunc (wrapped);
}

} // namespace Api

} // namespace Proc
