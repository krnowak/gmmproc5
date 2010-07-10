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
#include <fstream>
#include <memory>
#include <sstream>

// api
#include "enum.h"
#include "function.h"
#include "namespace.h"
#include "object.h"
#include "param.h"

// defsparser
#include "defsparser.h"
#include "statementizetask.h"

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

DefsParser::DefsParser (const std::string& path)
: m_tokens (),
  m_statements (),
  m_directory (dirname (path)),
  m_namespace (0),
  m_statement_actions (),
  m_parsed_files (std::deque<std::pair<std::string, std::string> > (1, std::make_pair (path, read_contents (path)))),
  m_types_ns ()
{
  const std::string c_name ("c-name");
  const std::string of_object ("of-object");
  const std::string empty;

  m_statement_actions["include"]                = std::bind (&DefsParser::on_include, this, std::placeholders::_1);
  m_statement_actions["define-flags-extended"]  = std::bind (&DefsParser::on_flags, this, std::placeholders::_1);
  m_statement_actions["define-enum-extended"]   = std::bind (&DefsParser::on_enum, this, std::placeholders::_1);
  m_statement_actions["define-object"]          = std::bind (&DefsParser::on_object, this, std::placeholders::_1);
  m_statement_actions["define-function"]        = std::bind (&DefsParser::on_function, this, std::placeholders::_1);
  m_statement_actions["define-method"]          = std::bind (&DefsParser::on_method, this, std::placeholders::_1);
  m_statement_actions["define-property"]        = std::bind (&DefsParser::on_property, this, std::placeholders::_1);
  m_statement_actions["define-signal"]          = std::bind (&DefsParser::on_signal, this, std::placeholders::_1);
  m_statement_actions["define-vfunc"]           = std::bind (&DefsParser::on_vfunc, this, std::placeholders::_1);
  m_statement_actions["define-flags"]           = std::bind (&DefsParser::on_omit, this, std::placeholders::_1);
  m_statement_actions["define-enum"]            = std::bind (&DefsParser::on_omit, this, std::placeholders::_1);
  
  m_types_ns["include"]               = empty;
  m_types_ns["define-flags-extended"] = c_name;
  m_types_ns["define-enum-extended"]  = c_name;
  m_types_ns["define-object"]         = c_name;
  m_types_ns["define-function"]       = c_name;
  m_types_ns["define-method"]         = c_name;
  m_types_ns["define-property"]       = of_object;
  m_types_ns["define-signal"]         = of_object;
  m_types_ns["define-vfunc"]          = of_object;
  m_types_ns["define-flags"]          = empty;
  m_types_ns["define-enum"]           = empty;
}

DefsParser::~DefsParser ()
{
  if (m_namespace)
  {
    delete m_namespace;
    m_namespace = 0;
  }
}

Api::Namespace* DefsParser::parse_vfunc ()
{
  Api::Namespace* ns (0);
  
  parse_round ();
  ns = m_namespace;
  m_namespace = 0;
  return ns;
}

std::string DefsParser::get_ext_vfunc () const
{
  return std::string ("defs");
}

void DefsParser::tokenize ()
{
  enum TokenType
  {
    TOKEN_NONE,
    TOKEN_DQ,
    TOKEN_STR,
    TOKEN_COMMENT
  } token_type (TOKEN_NONE);
  bool escape (false);
  std::list< std::string > tokens;
  std::string::iterator contents_end (m_parsed_files.top().second.end ());
  std::string::iterator token_begin (m_parsed_files.top().second.begin ());
  int current_line (0);
  
  for (std::string::iterator contents_iter (token_begin); contents_iter != contents_end; contents_iter++)
  {
    const char c (*contents_iter);
    
    switch (token_type)
    {
      case TOKEN_NONE:
      {
        bool push (false);
        bool begin (false);
        
        switch (c)
        {
          case '\n':
          {
            current_line++;
            // fall through
          }
          case '(':
          case ')':
          case '\'':
          {
            push = true;
            break;
          }
          case ';':
          {
            token_type = TOKEN_COMMENT;
            break;
          }
          case '"':
          {
            token_type = TOKEN_DQ;
            begin = true;
            break;
          }
          default:
          {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '-') || (c == '_') || (c == '#'))
            {
              token_type = TOKEN_STR;
              begin = true;
            }
            else if ((c != ' ') && (c != '\t'))
            {
              // syntax error
              return;
            }
          }
        }
        
        if (push)
        {
          m_tokens.push_back (std::string (1, c));
        }
        if (begin)
        {
          token_begin = contents_iter;
        }
        break;
      }
      case TOKEN_DQ:
      {
        switch (c)
        {
          case '"':
          {
            if (!escape)
            {
              m_tokens.push_back (std::string (token_begin, contents_iter + 1));
              token_type = TOKEN_NONE;
            }
            else
            {
              escape = false;
            }
            break;
          }
          case '\\':
          {
            escape = !escape;
            break;
          }
          case '\n':
          {
            // syntax error
            return;
          }
          default:
          {
            escape = false;
          }
        }
        break;
      }
      case TOKEN_STR:
      {
        bool push (false);
        bool decrement (false);
        
        switch (c)
        {
          case '(':
          case ')':
          case '\'':
          case '\n':
          case ';':
          case '"':
          {
            decrement = true;
            // fall through
          }
          case ' ':
          case '\t':
          {
            push = true;
            break;
          }
          default:
          {
            if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c != '-') && (c != '_') && (c != '#'))
            {
              // syntax error
              return;
            }
          }
        }
        if (push)
        {
          m_tokens.push_back (std::string (token_begin, contents_iter));
          token_type = TOKEN_NONE;
        }
        if (decrement)
        {
          contents_iter--;
        }
        break;
      }
      case TOKEN_COMMENT:
      {
        if (c == '\n')
        {
          token_type = TOKEN_NONE;
          contents_iter--;
        }
        break;
      }
      default:
      {
        // internal error
        return;
      }
    }
  }
}

void DefsParser::statementize ()
{
  std::auto_ptr<StatementizeTask> statementize_task (new StatementizeTask (m_parsed_files.top().first));
  m_statements = statementize_task->statementize (m_tokens);
}

void DefsParser::apicize ()
{
  std::list<Statement>::iterator statements_end (m_statements.end ());
  
  for (std::list<Statement>::iterator statement_iter (m_statements.begin ()); statement_iter != statements_end; statement_iter++)
  {
    const std::string type (statement_iter->get_type ());
    std::unordered_map<std::string, std::function<void (const Statement&)> >::iterator it (m_statement_actions.find (type));
    
    if (it != m_statement_actions.end ())
    {
      it->second(*statement_iter);
    }
    else
    {
      //error
      return;
    }
  }
}

void DefsParser::parse_round ()
{
  tokenize ();
  statementize ();
  apicize ();
}

std::string DefsParser::read_contents (const std::string& filename)
{
  std::ifstream       file  (filename.c_str ());
  std::ostringstream  oss;
  
  if (file.good())
  {
    file >> oss.rdbuf ();
  }
  return oss.str ();
}

std::string DefsParser::dirname (const std::string& filename)
{
  const size_t found (filename.find_last_of ("/"));
  
  if (found == std::string::npos)
  {
    return std::string ("./");
  }
  return filename.substr (0, found + 1);
}

void DefsParser::on_include (const Statement& statement)
{
  const std::string path (m_directory + statement.get_header ());
  m_parsed_files.push (FileStack::value_type (path, read_contents (path)));
  parse_round ();
  m_parsed_files.pop ();
}

void DefsParser::on_enum (const Statement& statement)
{
  create_enum_or_flags( statement, false );
}

void DefsParser::on_flags (const Statement& statement)
{
  create_enum_or_flags( statement, true );
}

void DefsParser::on_object (const Statement& statement)
{
  const std::string cname (statement.get_value ("c-name"));
  const std::string parent (statement.get_value ("parent"));
  const std::string gtype (statement.get_value ("gtype-id"));
  Api::Object* object (0);
  
  if( cname.empty() || gtype.empty() || parent.empty() )
  {
    // error
    return;
  }
  create_namespace (statement);
  object = new Api::Object (cname);
  if (!m_namespace->add_object (object))
  {
    delete object;
    object = m_namespace->get_object (cname);
    if (!object)
    {
      // error
      return;
    }
  }
  object->set_parent (parent);
  object->set_gtype (gtype);
}

void DefsParser::on_function (const Statement& statement)
{
  const std::string cname (statement.get_value ("c-name"));
  const std::string ret_type (statement.get_value ("return-type"));
  Api::Function* function (0);
  
  if (cname.empty () || ret_type.empty ())
  {
    // error
    return;
  }
  create_namespace (statement);
  function = new Api::Function (cname);
  if (!m_namespace->add_function (function))
  {
    delete function;
    function = m_namespace->get_function (cname);
    if (!function)
    {
      // error
      return;
    }
  }
  function->set_ret_type (ret_type);
  std::list<std::vector<std::string> >::const_iterator values_end (statement.get_list_end ("parameters"));
  for (std::list<std::vector<std::string> >::const_iterator values_iter = statement.get_list_begin ("parameters"); values_iter != values_end; values_iter++)
  {
    if (values_iter->size () != 2)
    {
      // error
      return;
    }
    const std::string param_name ((*values_iter)[1]);
    const std::string param_type ((*values_iter)[2]);
    if (param_name.empty () || param_type.empty ())
    {
      // error
      return;
    }
    Api::Param* param = new Api::Param (param_name);
    if (!function->append_param (param))
    {
      delete param;
      // error
      return;
    }
    param->set_type (param_type);
  }
}

void DefsParser::on_method (const Statement& /* statement */)
{

}

void DefsParser::on_property (const Statement& /* statement */)
{

}

void DefsParser::on_signal (const Statement& /* statement */)
{

}

void DefsParser::on_vfunc (const Statement& /* statement */)
{

}

void DefsParser::on_omit (const Statement& /* statement */)
{
  // empty
}
  
void DefsParser::create_enum_or_flags (const Statement& statement, bool flags)
{
  const std::string name ( statement.get_value ("c-name"));
  Api::Enum* enumeration ( 0 );
  
  if ( name.empty() )
  {
    // error
    return;
  }
  create_namespace (statement);
  enumeration = new Api::Enum (name);
  if ( !m_namespace->add_enum (enumeration))
  {
    delete enumeration;
    enumeration = 0;
    return;
  }
  enumeration->set_is_flags (flags);
  std::list<std::vector<std::string> >::const_iterator values_end (statement.get_list_end ("values"));
  for (std::list<std::vector<std::string> >::const_iterator values_iter = statement.get_list_begin ("values"); values_iter != values_end; values_iter++)
  {
    if (values_iter->size () != 3)
    {
      // error
      return;
    }
    const std::string elem_name ((*values_iter)[1]);
    const std::string elem_value ((*values_iter)[2]);
    if (elem_name.empty () || elem_value.empty ())
    {
      // error
      return;
    }
    Api::Enum::Element* element = new Api::Enum::Element (elem_name, elem_value);
    if (!enumeration->append_element (element))
    {
      delete element;
      // error
      return;
    }
  }
}

void DefsParser::create_namespace (const Statement& statement)
{
  const std::string parameter (m_types_ns[statement.get_type ()]);
  const std::string name = statement.get_value (parameter);
  const std::string ns_name = Api::Namespace::get_namespace_name (name);
  
  if (name.empty ())
  {
    return;
  }
  if (m_namespace)
  {
    if (m_namespace->get_id () != ns_name)
    {
      //error
    }
  }
  else
  {
    m_namespace = new Api::Namespace (ns_name);
  }
}

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc
