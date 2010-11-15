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
#include <unordered_map>

// api
#include "function.h"
#include "object.h"
#include "property.h"
#include "signal.h"

namespace Proc
{

namespace Api
{

struct Object::ObjectImpl
{
  typedef std::unordered_map<std::string, FunctionPtr> StringFunctionMap;
  typedef std::unordered_map<std::string, SignalPtr> StringSignalMap;
  typedef std::unordered_map<std::string, PropertyPtr> StringPropertyMap;
  
  ObjectImpl ();
  ObjectImpl (const std::string& id);

  std::string m_id;
  bool m_wrapped;
  std::string m_parent;
  std::string m_gtype;
  StringFunctionMap m_constructors;
  FunctionPtr m_destructor;
  StringFunctionMap m_methods;
  StringSignalMap m_signals;
  StringPropertyMap m_properties;
  StringFunctionMap m_vfuncs;
  bool m_is_interface;
};

Object::ObjectImpl::ObjectImpl ()
: m_id (),
  m_wrapped (false),
  m_parent (),
  m_gtype (),
  m_constructors (),
  m_destructor (),
  m_methods (),
  m_signals (),
  m_properties (),
  m_vfuncs (),
  m_is_interface (false)
{}

Object::ObjectImpl::ObjectImpl (const std::string& id)
: m_id (id),
  m_wrapped (false),
  m_parent (),
  m_gtype (),
  m_constructors (),
  m_destructor (),
  m_methods (),
  m_signals (),
  m_properties (),
  m_vfuncs (),
  m_is_interface (false)
{}

Object::Object ()
: Wrappable (),
  m_pimpl (new ObjectImpl)
{}

Object::Object (const std::string& id)
: Wrappable (),
  m_pimpl (new ObjectImpl (id))
{}

Object::~Object()
{}

std::string Object::get_parent () const
{
  return m_pimpl->m_parent;
}

void Object::set_parent (const std::string& parent)
{
  m_pimpl->m_parent = parent;
}

std::string Object::get_gtype () const
{
  return m_pimpl->m_gtype;
}

void Object::set_gtype (const std::string& gtype)
{
  m_pimpl->m_gtype = gtype;
}

void Object::add_constructor (const FunctionPtr& constructor)
{
  m_pimpl->m_constructors.insert (std::make_pair (constructor->get_id (), constructor));
}

void Object::set_destructor (const FunctionPtr& destructor)
{
  m_pimpl->m_destructor = destructor;
}

void Object::add_method (const FunctionPtr& method)
{
  m_pimpl->m_methods.insert (std::make_pair (method->get_id (), method));
//  m_pimpl->m_methods.push_back (method);
}

void Object::add_signal (const SignalPtr& signal)
{
  m_pimpl->m_signals.insert (std::make_pair (signal->get_id (), signal));
//  m_pimpl->m_signals.push_back (signal);
}

void Object::add_property (const PropertyPtr& property)
{
  m_pimpl->m_properties.insert (std::make_pair (property->get_id (), property));
//  m_pimpl->m_properties.push_back (property);
}

void Object::add_vfunc (const FunctionPtr& vfunc)
{
  m_pimpl->m_vfuncs.insert (std::make_pair (vfunc->get_id (), vfunc));
 // m_pimpl->m_constructors.push_back (constructor);
}

bool Object::get_is_interface () const
{
  return m_pimpl->m_is_interface;
}

void Object::set_is_interface (bool iface)
{
  m_pimpl->m_is_interface = iface;
}

void Object::swap (Object& object)
{
  m_pimpl.swap (object.m_pimpl);
}

std::string Object::get_id_vfunc () const
{
  return m_pimpl->m_id;
}

void Object::set_id_vfunc (const std::string& id)
{
  m_pimpl->m_id = id;
}

bool Object::get_wrapped_vfunc () const
{
  return m_pimpl->m_wrapped;
}

void Object::set_wrapped_vfunc (bool wrapped)
{
  m_pimpl->m_wrapped = wrapped;
}

} // namespace Api

} // namespace Proc
