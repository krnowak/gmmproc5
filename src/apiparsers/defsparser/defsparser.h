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

// defsprivate
#include "statement.h"

namespace Proc
{

namespace Api
{

class Namespace;

} // namespace Api

namespace Parsers
{

namespace ApiParsers
{

class DefsParser : public ApiParser
{
public:
  DefsParser (const std::string& path);
  virtual ~DefsParser ();

private:
  typedef std::unordered_map<std::string, std::function<void (const Statement&)> > StringFunctionMap;
  typedef std::stack<std::pair<std::string, std::string> > FileStack;
  typedef std::unordered_map<std::string, std::string> StringStringMap;
  
  virtual Api::Namespace*   parse_vfunc ();
  virtual std::string       get_ext_vfunc () const;

  void                      tokenize ();
  void                      statementize ();
  void                      apicize ();
  void                      parse_round ();
  std::string               read_contents (const std::string& path);
  std::string               dirname (const std::string& path);
  
  void                      on_include (const Statement& statement);
  void                      on_enum (const Statement& statement);
  void                      on_flags (const Statement& statement);
  void                      on_object (const Statement& statement);
  void                      on_function (const Statement& statement);
  void                      on_method (const Statement& statement);
  void                      on_property (const Statement& statement);
  void                      on_signal (const Statement& statement);
  void                      on_vfunc (const Statement& statement);
  void                      on_omit (const Statement& statement);
  
  void						          create_enum_or_flags (const Statement& statement, bool flags);
  void						          create_namespace (const Statement& statement);

  std::list<std::string>    m_tokens;
  std::list<Statement>      m_statements;
  std::string               m_directory;
  Api::Namespace*           m_namespace;
  StringFunctionMap         m_statement_actions;
  FileStack                 m_parsed_files;
  StringStringMap           m_types_ns;
};

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc

#endif // PROC_PARSERS_API_PARSERS_DEFS_PARSER_H
