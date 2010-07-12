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

#ifndef PROC_PARSERS_API_PARSERS_STATEMENTIZE_TASK_H
#define PROC_PARSERS_API_PARSERS_STATEMENTIZE_TASK_H

// standard
#include <functional>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// defsprivate
#include "statement.h"

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

class StatementizeTask
{
public:
                                                  StatementizeTask (const std::string& file);
  std::list<Statement>                            statementize (const std::list<std::string>& tokens);
private:
  typedef std::unordered_map<std::string, std::function<void()> > StringFunctionMap;

  void                                            on_token_newline ();
  void                                            on_token_open_paren ();
  void                                            on_token_close_paren ();
  void                                            on_token_apostrophe ();
  void                                            on_token_other (const std::string& token);
  void                                            throw_syntax_error (const std::string& what_arg);

  enum Context
  {
    CONTEXT_OUTSIDE,
    CONTEXT_HEADER,
    CONTEXT_LEVEL1,
    CONTEXT_LEVEL2,
    CONTEXT_BEFORE_LIST_ELEMENTS,
    CONTEXT_LIST_ELEMENTS
  };

  Statement                                       m_current_statement;
  Context                                         m_current_context;
  int                                             m_long_token_num;
  int                                             m_list_elements_count;
  std::string                                     m_value;
  std::string                                     m_key;
  std::list<std::vector<std::string> >            m_element_list;
  std::vector<std::string>                        m_elements;
  StringFunctionMap                               m_token_actions;
  int                                             m_current_line;
  std::list<Statement>                            m_statements;
  std::string                                     m_file;
};

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc

#endif // PROC_PARSERS_API_PARSERS_STATEMENTIZE_TASK_H

