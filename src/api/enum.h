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

#ifndef PROC_API_ENUM_H
#define PROC_API_ENUM_H

// standard
#include <list>
#include <string>
#include <utility>
// api
#include "id.h"

namespace Proc
{

namespace Api
{

class Enum : public Id
{
public:
  typedef std::pair<std::string, std::string> Element;

  Enum (const std::string& id = std::string());
  
  Enum (const Enum& enumeration);
  Enum& operator= (const Enum& enumeration);
  
  virtual ~Enum();
  
  inline bool is_flags () const;
  inline void set_is_flags (bool is_flags = true);
  
  inline std::list<Element*>::const_iterator get_begin () const;
  inline std::list<Element*>::iterator get_begin ();
  inline std::list<Element*>::const_iterator get_end () const;
  inline std::list<Element*>::iterator get_end ();
  
  bool append_element (Element* element);
  inline std::list<Element*>::iterator erase(const std::list<Element*>::iterator& position);
  
  inline bool operator== (const Enum& enumeration) const;
private:
  std::list<Element* > m_elements;
  bool m_is_flags;
};

inline bool Enum::is_flags () const
{
  return m_is_flags;
}

inline void Enum::set_is_flags (bool is_flags)
{
  m_is_flags = is_flags;
}

inline std::list<Enum::Element*>::const_iterator Enum::get_begin () const
{
  return m_elements.begin ();
}

inline std::list<Enum::Element*>::iterator Enum::get_begin ()
{
  return m_elements.begin ();
}

inline std::list<Enum::Element*>::const_iterator Enum::get_end () const
{
  return m_elements.end ();
}

inline std::list<Enum::Element*>::iterator Enum::get_end ()
{
  return m_elements.end ();
}

inline std::list<Enum::Element*>::iterator Enum::erase(const std::list<Enum::Element*>::iterator& position)
{
  return m_elements.erase (position);
}

inline bool Enum::operator== (const Enum& enumeration) const
{
  const Id* id (this);
  return (*id == enumeration);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_ENUM_H
