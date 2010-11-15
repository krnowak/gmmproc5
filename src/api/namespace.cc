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
#include <functional>
#include <unordered_map>

// api
#include "enum.h"
#include "function.h"
#include "namespace.h"
#include "object.h"

namespace Proc
{

namespace Api
{

struct Namespace::NamespaceImpl
{
  typedef std::unordered_map<std::string, EnumPtr> StringEnumMap;
  typedef std::unordered_map<std::string, ObjectPtr> StringObjectMap;
  typedef std::unordered_map<std::string, FunctionPtr> StringFunctionMap;

  NamespaceImpl ();
  NamespaceImpl (const std::string& id);

  static bool underline_break (const std::string::const_iterator& it);
  static bool capital_break (const std::string::const_iterator& it);

  std::string m_id;
  StringEnumMap m_enums;
  StringObjectMap m_objects;
  StringFunctionMap m_functions;
};

Namespace::NamespaceImpl::NamespaceImpl ()
: m_id (),
  m_enums (),
  m_objects (),
  m_functions ()
{}

Namespace::NamespaceImpl::NamespaceImpl (const std::string& id)
: m_id (id),
  m_enums (),
  m_objects (),
  m_functions ()
{}

bool Namespace::NamespaceImpl::underline_break (const std::string::const_iterator& it)
{
  return (*it != '_');
}

bool Namespace::NamespaceImpl::capital_break (const std::string::const_iterator& it)
{
  return ((*it < 'A') || (*it > 'Z'));
}

Namespace::Namespace ()
: Identifiable (),
  m_pimpl (new NamespaceImpl)
{}

Namespace::Namespace (const std::string& id)
: Identifiable (),
  m_pimpl (new NamespaceImpl (id))
{}

Namespace::~Namespace ()
{}

void Namespace::add_function (const FunctionPtr& function)
{
  m_pimpl->m_functions.insert (std::make_pair (function->get_id (), function));
}

FunctionPtr Namespace::get_function (const std::string& c_name) const
{
  NamespaceImpl::StringFunctionMap::const_iterator it (m_pimpl->m_functions.find (c_name));

  if (it != m_pimpl->m_functions.end ())
  {
    return it->second;
  }
  return FunctionPtr ();
}

void Namespace::add_object (const ObjectPtr& object)
{
  m_pimpl->m_objects.insert (std::make_pair (object->get_id (), object));
}

ObjectPtr Namespace::get_object (const std::string& c_name) const
{
  NamespaceImpl::StringObjectMap::const_iterator it (m_pimpl->m_objects.find (c_name));

  if (it != m_pimpl->m_objects.end ())
  {
    return it->second;
  }
  return ObjectPtr ();
}

void Namespace::add_enum (const EnumPtr& enumeration)
{
  m_pimpl->m_enums.insert (std::make_pair (enumeration->get_id (), enumeration));
}

EnumPtr Namespace::get_enum (const std::string& c_name) const
{
  NamespaceImpl::StringEnumMap::const_iterator it (m_pimpl->m_enums.find (c_name));

  if (it != m_pimpl->m_enums.end ())
  {
    return it->second;
  }
  return EnumPtr ();
}
std::string Namespace::get_namespace_name (const std::string& name)
{
  std::string::const_iterator it;
  std::string ns_name;
  std::function<bool (const std::string::const_iterator&)> break_func;
  bool grow (false);

  if (name.empty ())
  {
    return std::string ();
  }
  it = name.begin ();
  if ((*it >= 'A') && (*it <= 'Z'))
  {
    break_func = std::bind (&Namespace::NamespaceImpl::capital_break, std::placeholders::_1);
  }
  else if ((*it >= 'a') && (*it <= 'z'))
  {
    break_func = std::bind (&Namespace::NamespaceImpl::underline_break, std::placeholders::_1);
    grow = true;
  }
  else
  {
    return std::string ();
  }
  do
  {
    ++it;
  }
  while ((it != name.end ()) && break_func (it));
  if (it == name.end ())
  {
    return std::string ();
  }
  ns_name = std::string (name.begin (), it);
  if (grow)
  {
    ns_name[0] += 'A' - 'a';
  }
  return ns_name;
}

void Namespace::swap (Namespace& ns)
{
  m_pimpl.swap (ns.m_pimpl);
}

std::string Namespace::get_id_vfunc () const
{
  return m_pimpl->m_id;
}

void Namespace::set_id_vfunc (const std::string& id)
{
  m_pimpl->m_id = id;
}

} // namespace Api

} // namespace Proc
