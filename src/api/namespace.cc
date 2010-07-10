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
#include <algorithm>
#include <functional>
// common
#include "apitemplates.h"
#include "stlops.h"
// api
#include "enum.h"
#include "function.h"
#include "namespace.h"
#include "object.h"

namespace Proc
{

namespace Api
{

Namespace::Namespace (const std::string& id)
: Id (id),
  m_enums (),
  m_objects (),
  m_functions ()
{}

Namespace::~Namespace ()
{
  std::for_each (m_enums.begin (), m_enums.end (), ::Proc::Common::PairDeleter<std::string, Enum> ());
  std::for_each (m_objects.begin (), m_objects.end (), ::Proc::Common::PairDeleter<std::string, Object> ());
  std::for_each (m_functions.begin (), m_functions.end (), ::Proc::Common::PairDeleter<std::string, Function> ());
}

bool Namespace::add_function (Function* function)
{
  return ::Proc::Common::IdInserter<Function> () (m_functions, function);
}

Function* Namespace::get_function (const std::string& c_name) const
{
  std::map<std::string, Function*>::const_iterator it (m_functions.find (c_name));
  if (it != m_functions.end ())
  {
    return it->second;
  }
  return 0;
}

bool Namespace::add_object (Object* object)
{
  return ::Proc::Common::IdInserter<Object> () (m_objects, object);
}

Object* Namespace::get_object (const std::string& c_name) const
{
  std::map<std::string, Object*>::const_iterator it (m_objects.find (c_name));
  if (it != m_objects.end ())
  {
    return it->second;
  }
  return 0;
}

bool Namespace::add_enum (Enum* enumeration)
{
  return ::Proc::Common::IdInserter<Enum> () (m_enums, enumeration);
}

Enum* Namespace::get_enum (const std::string& c_name) const
{
  std::map<std::string, Enum*>::const_iterator it (m_enums.find (c_name));
  if (it != m_enums.end ())
  {
    return it->second;
  }
  return 0;
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
    break_func = std::bind (&Namespace::capital_break, std::placeholders::_1);
  }
  else if ((*it >= 'a') && (*it <= 'z'))
  {
    break_func = std::bind (&Namespace::underline_break, std::placeholders::_1);
    grow = true;
  }
  else
  {
    return std::string ();
  }
  do
  {
    it++;
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

bool Namespace::underline_break (const std::string::const_iterator& it)
{
  return (*it != '_');
}

bool Namespace::capital_break (const std::string::const_iterator& it)
{
  return ((*it < 'A') || (*it > 'Z'));
}

} // namespace Api

} // namespace Proc

