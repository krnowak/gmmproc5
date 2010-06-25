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

#ifndef PROC_API_OBJECT_H
#define PROC_API_OBJECT_H

// standard
#include <map>
#include <string>
// api
#include "id.h"

namespace Proc
{

namespace Api
{

class Function;
class Property;
class Signal;

class Object : public Id
{
public:
  Object (const std::string& id = std::string ());
  
  Object (const Object& object);
  Object& operator= (const Object& object);
  
  virtual ~Object ();
  
  inline std::string get_parent () const;
  bool set_parent ( const std::string& parent );
  
  inline std::string get_gtype () const;
  bool set_gtype ( const std::string& gtype );
  
  bool add_constructor (Function* constructor);
  bool set_destructor (Function* destructor);
  bool add_method (Function* method);
  bool add_signal (Signal* signal);
  bool add_property (Property* property);
  bool add_vfunc (Function* vfunc);
  
  inline bool operator== (const Object& object);
private:
  std::string m_parent;
  std::string m_gtype;
  std::map<std::string, Function*> m_constructors;
  Function* m_destructor;
  std::map<std::string, Function*> m_methods;
  std::map<std::string, Signal*> m_signals;
  std::map<std::string, Property*> m_properties;
  std::map<std::string, Function*> m_vfuncs;
};

inline std::string Object::get_parent () const
{
  return m_parent;
}

inline std::string Object::get_gtype () const
{
  return m_gtype;
}

inline bool Object::operator== (const Object& object)
{
  return (*static_cast<Id*> (this) == object);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_OBJECT_H
