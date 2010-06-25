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

namespace DefsPrivate
{

Statement::Statement()
  m_type (),
  m_header (),
  m_values (),
  m_bools (),
  m_lists ()
{}

Statement::~Statement()
{}

void Statement::clear()
{
  m_type.clear ();
  m_header.clear ();
  m_values.clear ();
  m_bools.clear ();
  m_lists.clear ();
}

std::string Statement::extract_header() const
{
  size_t bidx (m_body.find ("("));
  size_t eidx (std::string::npos);
  
  if (bidx == std::string::npos)
  {
    return std::string();
  }
  bidx = m_body.find (" ", bidx + 1);
  if (bidx == std::string::npos)
  {
    return std::string ();
  }
  bidx++;
  eidx = m_body.find_first_of (" )", bidx);
  if (eidx == std::string::npos)
  {
    return std::string ();
  }
  return m_body.substr (bidx, eidx - bidx);
}

std::string Statement::extract_value( const std::string& key )
{
  size_t bidx (get_key_value_index (key));
  
  if ((bidx != std::string::npos) && (m_body[bidx] == '"'))
  {
    size_t eidx (std::string::npos);
    
    do
    {
      eidx = m_body.find ("\"", bidx);
      if (eidx == std::string::npos)
      {
        return std::string();
      }
    }
    while ((m_body[eidx - 1] != '\\') || (m_body[eidx - 2] == '\\'));
    return m_body.substr (bidx, eidx - bidx);
  }
  return std::string();
}

bool Statement::extract_bool( const std::string& key )
{
  size_t bidx (get_key_value_index (key));
  
  if ((bidx != std::string::npos) && (m_body[bidx] == '#'))
  {
    size_t eidx = bidx + 1;
    
    if ((eidx < m_body.length()) && (m_body[eidx] == 't'))
    {
      return true;
    }
  }
  return false;
}

std::list< std::vector< std::string> > Statement::extract_value_list (const std::string& key)
{
  size_t bidx (get_key_value_index (key));
  std::list< std::vector< std::string> > list;
  
  if (bidx != std::string::npos)
  {
    do
    {
      bidx = m_body.find ("'(", bidx);
    }
  }
  return list;
}

size_t Statement::get_key_value_index (const std::string& key)
{
  const size_t len (m_body.length());
  const size_t keylen (key.length());
  
  size_t idx (std::string::npos);
  bool inside_dq (false);
  bool begin_key (false);
  bool found (false);
  int level = 0;
  
  for (unsigned int iter = 0; (iter < len) && !found; iter++)
  {
    char c = body [iter];
    if (inside_dq)
    {
      if (c == '"' && body [iter - 1] != '\\')
      {
        inside_dq = false;
      }
    }
    else if (c == '"')
    {
      inside_dq = true;
    }
    else if (c == '(')
    {
      level++;
      if (level == 2)
      {
        begin_key = true;
      }
    }
    else if (c == ')')
    {
      level--;
    }
    else if (begin_key)
    {
      begin_key = false;
      if ((iter + keylen) < len)
      {
        if (body.substr (iter, keylen + 1) == (key + ' '))
        {
          idx = body.find_first_not_of (' ', iter + keylen);
          return idx;
        }
      }
      else
      {
        return std::string::npos;
      }
    }
  }
  return std::string::npos;
}

} // namespace DefsPrivate

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc
