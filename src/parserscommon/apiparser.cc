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

// parserscommon
#include "apiparser.h"

namespace Proc
{

namespace Parsers
{

ApiParser::~ApiParser ()
{}

void ApiParser::set_file_to_parse (const std::string& file_path)
{
  set_file_to_parse_vfunc (file_path);
}

void ApiParser::set_string_to_parse (const std::string& contents)
{
  set_string_to_parse_vfunc (contents);
}

Api::NamespacePtr ApiParser::parse ()
{
  return parse_vfunc();
}

std::string ApiParser::get_ext ()
{
  return get_ext_vfunc ();
}

void ApiParser::reset ()
{
  reset_vfunc ();
}

} // namespace Parsers

} // namespace Proc
