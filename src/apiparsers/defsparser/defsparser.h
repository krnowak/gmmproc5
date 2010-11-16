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

#ifndef PROC_PARSERS_API_PARSERS_DEFS_PARSER_H
#define PROC_PARSERS_API_PARSERS_DEFS_PARSER_H

// standard
#include <list>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

// parsers
#include "apiparser.h"

// defsparser
#include "statement.h"

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

class DefsParser : public ApiParser
{
public:
  DefsParser ();
  virtual ~DefsParser ();

private:
  struct DefsParserImpl;
  std::shared_ptr<DefsParserImpl> m_pimpl;

  virtual void set_file_to_parse_vfunc (const std::string& file_path);
  virtual void set_string_to_parse_vfunc (const std::string& contents);
  virtual Api::NamespacePtr parse_vfunc ();
  virtual std::string get_ext_vfunc () const;
  virtual void reset_vfunc ();
};

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc

#endif // PROC_PARSERS_API_PARSERS_DEFS_PARSER_H
