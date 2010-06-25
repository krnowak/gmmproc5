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

#ifndef PROC_PARSERS_APIPARSER_H
#define PROC_PARSERS_APIPARSER_H

namespace Proc
{

class Api::Namespace;

namespace Parsers
{

class ApiParser
{
public:
  virtual ~ApiParser () {}
  inline Api::Namespace* parse ();
  inline std::string get_ext () const;
private:
  virtual Api::Namespace* parse_vfunc () = 0;
  virtual std::string get_ext_vfunc () const = 0;
};

inline Api::Namespace* ApiParser::parse()
{
  return parse_vfunc();
}

inline std::string ApiParser::get_ext () const
{
  return get_ext_vfunc ();
}

} // namespace Parsers

} // namespace Proc

#endif // PROC_PARSERS_APIPARSERS_H
