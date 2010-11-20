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

#include "token.h"

namespace Proc
{

namespace Template
{

struct Token::TokenImpl
{
  TokenImpl (Type type, int line, const std::string& contents);

  Type m_type;
  unsigned int m_line;
  std::string m_contents;
};

Token::TokenImpl::TokenImpl (Token::Type type, int line, const std::string& contents)
: m_type (type),
  m_line (line),
  m_contents (contents)
{}

Token::Token (Type type, int line, const std::string& contents)
: m_pimpl (new TokenImpl (type, line, contents))
{}

Token::Type Token::get_type () const
{
  return m_pimpl->m_type;  
}

unsigned int Token::get_line () const
{
  return m_pimpl->m_line;
}

std::string Token::get_contents () const
{
  return m_pimpl->m_contents;
}

} // namespace Template

} // namespace Proc
