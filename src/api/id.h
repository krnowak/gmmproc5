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

#ifndef PROC_API_ID_H
#define PROC_API_ID_H

// standard
#include <string>

namespace Proc
{

namespace Api
{

class Id
{
public:
  Id (const std::string& id = std::string ());

  virtual ~Id();

  inline std::string get_id () const;
  bool set_id (const std::string& id);
  
  inline bool operator== (const Id& id) const;
private:
  std::string m_id;
};

inline std::string Id::get_id () const
{
  return m_id;
}

inline bool Id::operator== (const Id& id) const
{
  return (m_id == id.m_id);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_ID_H
