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
#include <string>
#include <unordered_map>

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
  Object&             operator= (const Object& object);

  virtual             ~Object ();

  inline std::string  get_parent () const;
  bool                set_parent ( const std::string& parent );

  inline std::string  get_gtype () const;
  bool                set_gtype ( const std::string& gtype );

  bool                add_constructor (Function* constructor);
  bool                set_destructor (Function* destructor);
  bool                add_method (Function* method);
  bool                add_signal (Signal* signal);
  bool                add_property (Property* property);
  bool                add_vfunc (Function* vfunc);

  inline bool         is_interace () const;
  inline void         set_is_interface (bool iface);

  inline bool         operator== (const Object& object);
private:
  typedef std::unordered_map<std::string, Function*> StringFunctionMap;
  typedef std::unordered_map<std::string, Signal*> StringSignalMap;
  typedef std::unordered_map<std::string, Property*> StringPropertyMap;

  std::string         m_parent;
  std::string         m_gtype;
  StringFunctionMap   m_constructors;
  Function*           m_destructor;
  StringFunctionMap   m_methods;
  StringSignalMap     m_signals;
  StringPropertyMap   m_properties;
  StringFunctionMap   m_vfuncs;
  bool                m_is_interface;
};

inline std::string Object::get_parent () const
{
  return m_parent;
}

inline std::string Object::get_gtype () const
{
  return m_gtype;
}

inline bool Object::is_interace () const
{
  return m_is_interface;
}

inline void Object::set_is_interface (bool iface)
{
  m_is_interface = iface;
}

inline bool Object::operator== (const Object& object)
{
  return (*static_cast<Id*> (this) == object);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_OBJECT_H
