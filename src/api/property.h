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

#ifndef PROC_API_PROPERTY_H
#define PROC_API_PROPERTY_H

// standard
#include <string>

// api
#include "id.h"

namespace Proc
{

namespace Api
{

enum ParamFlags
{
  PARAM_INVALID = 0,
  PARAM_READABLE = 1 << 0,
  PARAM_WRITABLE = 1 << 1,
  PARAM_CONSTRUCT = 1 << 2,
  PARAM_CONSTRUCT_ONLY = 1 << 3
};

inline ParamFlags
operator& (ParamFlags first, ParamFlags second)
{
  return ParamFlags (static_cast<int> (first) & static_cast<int> (second));
}

inline ParamFlags
operator| (ParamFlags first, ParamFlags second)
{
  return ParamFlags (static_cast<int> (first) | static_cast<int> (second));
}

inline ParamFlags
operator^ (ParamFlags first, ParamFlags second)
{
  return ParamFlags (static_cast<int> (first) ^ static_cast<int> (second));
}

inline ParamFlags&
operator|= (ParamFlags& first, ParamFlags second)
{
  return first = first | second;
}

inline ParamFlags&
operator&= (ParamFlags& first, ParamFlags second)
{
  return first = first & second;
}

inline ParamFlags&
operator^= (ParamFlags& first, ParamFlags second)
{
  return first = first ^ second;
}

inline ParamFlags
operator~ (ParamFlags first)
{
  return ParamFlags (~static_cast<int> (first));
}

class Property : public Id
{
public:
                    Property (const std::string& id = std::string ());

  virtual           ~Property ();

  std::string       get_type () const;
  bool              set_type (const std::string& type);

  inline ParamFlags get_flags () const;
  bool              set_flags (ParamFlags param_flags);

  inline bool       operator== (const Property& property) const;
private:
  std::string       m_type;
  ParamFlags        m_param_flags;
};

inline std::string Property::get_type() const
{
  return m_type;
}

inline ParamFlags Property::get_flags() const
{
  return m_param_flags;
}

inline bool Property::operator== (const Property& property) const
{
  const Id* id (this);
  return (*id == property);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_PROPERTY_H
