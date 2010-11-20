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

#ifndef PROC_TEMPLATE_TOKEN_H
#define PROC_TEMPLATE_TOKEN_H

#include <memory>
#include <string>

namespace Proc
{

namespace Template
{

class Token
{
public:
  enum Type
  {
    TOKEN_WORD, // any word containing alnums, _ and #
    TOKEN_MULTILINE_COMMENT, // /* ajwaj */
    TOKEN_SINGLE_LINE_COMMENT, // // ajwaj
    TOKEN_DOXYGEN, // /** ajwaj */
    TOKEN_MISC, // parens, braces, newlines, semicolons and commas
    TOKEN_DOUBLY_QUOTED_STRING, // "ajwaj"
    TOKEN_SINGLY_QUOTED_STRING, // 'a'
    TOKEN_FUNNY_QUOTED_STRING, // `ajwaj'
    TOKEN_OTHER, // other tokens (whitespace, dots, angle brackets and so on).
    TOKEN_INVALID // invalid token
  };

  Token (Type type, int line, const std::string& contents);

  Type get_type () const;
  unsigned int get_line () const;
  std::string get_contents () const;

private:
  struct TokenImpl;
  std::shared_ptr<TokenImpl> m_pimpl;
};

} // namespace Template

} // namespace Proc

#endif // PROC_TEMPLATE_TOKEN_H
