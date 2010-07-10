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

#ifndef PROC_API_FUNCTION_H
#define PROC_API_FUNCTION_H

// standard
#include <list>
#include <string>

// api
#include "id.h"

namespace Proc
{

namespace Api
{

class Param;

class Function : public Id
{
public:
                                            Function (const std::string& id = std::string ());

                                            Function (const Function& function);
  Function&                                 operator= (const Function& function);

  virtual                                   ~Function ();

  inline std::string                        get_ret_type () const;
  bool                                      set_ret_type (const std::string& ret_type);

  inline std::list<Param*>::const_iterator  get_begin () const;
  inline std::list<Param*>::iterator        get_begin ();
  inline std::list<Param*>::const_iterator  get_end () const;
  inline std::list<Param*>::iterator        get_end ();

  bool                                      append_param (Param* param);
  inline std::list<Param*>::iterator        erase (const std::list<Param*>::iterator& position);

  inline bool                               has_varargs () const;
  inline void                               set_varargs (bool has_varargs = true);

  inline bool                               operator== (const Function& function) const;
private:
  std::string                               m_ret_type;
  std::list<Param*>                         m_parameters;
  bool                                      m_has_varargs;
};

inline std::string Function::get_ret_type () const
{
  return m_ret_type;
}

inline std::list<Param*>::const_iterator Function::get_begin () const
{
  return m_parameters.begin ();
}

inline std::list<Param*>::iterator Function::get_begin ()
{
  return m_parameters.begin ();
}

inline std::list<Param*>::const_iterator Function::get_end () const
{
  return m_parameters.end ();
}

inline std::list<Param*>::iterator Function::get_end ()
{
  return m_parameters.end ();
}

inline std::list<Param*>::iterator Function::erase(const std::list<Param*>::iterator& position)
{
  return m_parameters.erase (position);
}

inline bool Function::has_varargs () const
{
  return m_has_varargs;
}

inline void Function::set_varargs (bool has_varargs)
{
  m_has_varargs = has_varargs;
}

inline bool Function::operator== (const Function& function) const
{
  const Id* id (this);
  return (*id == function);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_FUNCTION_H
