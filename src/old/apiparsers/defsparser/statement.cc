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

#include "statement.h"

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

Statement::Statement ()
: m_type (),
  m_header (),
  m_values (),
  m_bools (),
  m_lists ()
{}

Statement::~Statement ()
{}

std::string Statement::get_type () const
{
  return m_type;
}

void Statement::set_type (const std::string& type)
{
  m_type = type;
}

std::string Statement::get_header () const
{
  return m_header;
}

void Statement::set_header (const std::string& header)
{
  m_header = header;
}

Statement::StringBoolPair Statement::get_value (const std::string& key) const
{
  StringStringMap::const_iterator it (m_values.find (key));

  if (it == m_values.end ())
  {
    return StringBoolPair(std::string (), false);
  }
  return StringBoolPair(it->second, true);
}

void Statement::set_value (const std::string& key, const std::string& value)
{
  m_values.insert (StringStringMap::value_type (key, value));
}

Statement::BoolBoolPair Statement::get_bool (const std::string& key) const
{
  StringBoolMap::const_iterator it (m_bools.find (key));

  if (it == m_bools.end ())
  {
    return BoolBoolPair (false, false);
  }
  return BoolBoolPair (it->second, true);
}

void Statement::set_bool (const std::string& key, bool value)
{
  m_bools.insert (StringBoolMap::value_type (key, value));
}

Statement::IteratorBoolPair Statement::get_list_begin (const std::string& key)
{
  StringListMap::iterator it (m_lists.find (key));

  if (it == m_lists.end ())
  {
    return IteratorBoolPair (std::list<std::vector<std::string>>::iterator (), false);
  }
  return IteratorBoolPair (it->second.begin (), true);
}

Statement::ConstIteratorBoolPair Statement::get_list_begin (const std::string& key) const
{
  return const_cast<Statement*> (this)->get_list_begin (key);
}

Statement::IteratorBoolPair Statement::get_list_end (const std::string& key)
{
  StringListMap::iterator it(m_lists.find (key));

  if (it == m_lists.end ())
  {
    return IteratorBoolPair (std::list<std::vector<std::string> >::iterator(), false);
  }
  return IteratorBoolPair (it->second.end (), true);
}

Statement::ConstIteratorBoolPair Statement::get_list_end (const std::string& key) const
{
  return const_cast<Statement*> (this)->get_list_end (key);
}

void Statement::set_list (const std::string& key,const std::list<std::vector<std::string> >& list)
{
  m_lists.insert (StringListMap::value_type (key, list));
}

std::string Statement::get_file () const
{
  return m_file;
}

void Statement::set_file (const std::string& file)
{
  m_file = file;
}

unsigned int Statement::get_line () const
{
  return m_line;
}

void Statement::set_line (unsigned int line)
{
  m_line = line;
}

void Statement::clear ()
{
  m_type.clear ();
  m_header.clear ();
  m_values.clear ();
  m_bools.clear ();
  m_lists.clear ();
  m_file.clear ();
  m_line = 0;
}

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc
