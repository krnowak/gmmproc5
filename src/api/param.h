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

#ifndef PROC_API_PARAM_H
#define PROC_API_PARAM_H

// standard
#include <string>
// api
#include "id.h"

namespace Proc
{

namespace Api
{

class Param : public Id
{
public:
  Param (const std::string& id = std::string ());
  
  virtual ~Param ();
  
  inline std::string get_type () const;
  bool set_type (const std::string& type);
private:
  std::string m_type;
};

std::string Param::get_type () const
{
  return m_type;
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_PARAM_H
