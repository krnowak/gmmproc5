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

// common
#include "internalerror.h"

// api
#include "enum.h"
#include "function.h"
#include "namespace.h"
#include "object.h"
#include "param.h"

// parsers
#include "syntaxerror.h"

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
: m_tokens (std::deque<TokensStack::value_type> (1)),
  m_statements (std::deque<StatementsStack::value_type> (1)),
  m_directory (dirname (path)),
  m_namespace (0),
  m_statement_actions (),
  m_parsed_files (std::deque<FileStack::value_type> (1, FileStack::value_type (path, read_contents (path)))),
  m_types_ns ()
{
  const std::string c_name ("c-name");
  const std::string of_object ("of-object");
  const std::string empty;

  m_statement_actions["include"] = std::bind (&DefsParser::on_include, this, std::placeholders::_1);
  m_statement_actions["define-flags-extended"] = std::bind (&DefsParser::on_flags, this, std::placeholders::_1);
  m_statement_actions["define-enum-extended"] = std::bind (&DefsParser::on_enum, this, std::placeholders::_1);
  m_statement_actions["define-object"] = std::bind (&DefsParser::on_object, this, std::placeholders::_1);
  m_statement_actions["define-function"] = std::bind (&DefsParser::on_function, this, std::placeholders::_1);
  m_statement_actions["define-method"] = std::bind (&DefsParser::on_method, this, std::placeholders::_1);
  m_statement_actions["define-property"] = std::bind (&DefsParser::on_property, this, std::placeholders::_1);
  m_statement_actions["define-signal"] = std::bind (&DefsParser::on_signal, this, std::placeholders::_1);
  m_statement_actions["define-vfunc"] = std::bind (&DefsParser::on_vfunc, this, std::placeholders::_1);
  m_statement_actions["define-flags"] = std::bind (&DefsParser::on_omit, this, std::placeholders::_1);
  m_statement_actions["define-enum"] = std::bind (&DefsParser::on_omit, this, std::placeholders::_1);

  m_types_ns["include"] = empty;
  m_types_ns["define-flags-extended"] = c_name;
  m_types_ns["define-enum-extended"] = c_name;
  m_types_ns["define-object"] = c_name;
  m_types_ns["define-function"] =c_name;
  m_types_ns["define-method"] = c_name;
  m_types_ns["define-property"] = of_object;
  m_types_ns["define-signal"] = of_object;
  m_types_ns["define-vfunc"] = of_object;
  m_types_ns["define-flags"] = empty;
  m_types_ns["define-enum"] = empty;
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
  const std::string file (m_parsed_files.top ().first);
  std::string::iterator contents_end (m_parsed_files.top ().second.end ());
  std::string::iterator token_begin (m_parsed_files.top ().second.begin ());
  int current_line (1);

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
              std::ostringstream oss;

              oss << "Illegal char: `" << c << "'.";
              throw SyntaxError (file, current_line, oss.str ());
            }
          }
        }

        if (push)
        {
          m_tokens.top ().push_back (std::string (1, c));
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
              m_tokens.top ().push_back (std::string (token_begin, contents_iter + 1));
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
            throw SyntaxError (file, current_line, "Unexpected end of line inside a double-quoted token.");
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
            if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9') && (c != '-') && (c != '_') && (c != '#') && (c != '.'))
            {
              std::ostringstream oss;

              oss << "Illegal char inside string token: `" << c << "'.";
              throw SyntaxError (file, current_line, oss.str ());
            }
          }
        }
        if (push)
        {
          m_tokens.top ().push_back (std::string (token_begin, contents_iter));
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
        std::ostringstream oss;

        oss << "Unknown TokenType value: `" << token_type << "'.";
        throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
      }
    }
  }
}

void DefsParser::statementize ()
{
  const std::string path (m_parsed_files.top ().first);
  std::auto_ptr<StatementizeTask> statementize_task (new StatementizeTask (path));

  m_statements.push (statementize_task->statementize (m_tokens.top ()));
}

void DefsParser::apicize ()
{
  std::list<Statement>::iterator statements_end (m_statements.top ().end ());

  for (std::list<Statement>::iterator statement_iter (m_statements.top ().begin ()); statement_iter != statements_end; statement_iter++)
  {
    const std::string type (statement_iter->get_type ());
    StringFunctionMap::iterator it (m_statement_actions.find (type));

    if (it != m_statement_actions.end ())
    {
      it->second (*statement_iter);
    }
    else
    {
      std::ostringstream oss;

      oss << "Unknown statement type: `" << statement_iter->get_type () << "'.";
      throw SyntaxError (statement_iter->get_file (), statement_iter->get_line (), oss.str ());
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
  std::ifstream file (filename.c_str ());
  std::ostringstream oss;

  if (file.good ())
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
  m_tokens.push (TokensStack::value_type ());
  // new statements is pushed in statementize method
  parse_round ();
  m_statements.pop ();
  m_tokens.pop ();
  m_parsed_files.pop ();
}

void DefsParser::on_enum (const Statement& statement)
{
  create_enum_or_flags (statement, false);
}

void DefsParser::on_flags (const Statement& statement)
{
  create_enum_or_flags (statement, true);
}

void DefsParser::on_object (const Statement& statement)
{
  const Statement::StringBoolPair cname (statement.get_value ("c-name"));
  const Statement::StringBoolPair parent (statement.get_value ("parent"));
  const Statement::StringBoolPair gtype (statement.get_value ("gtype-id"));
  Api::Object* object (0);
  bool is_interface (false);

  if (!cname.second)
  {
    throw SyntaxError (statement.get_file (),statement.get_line (), "No `c-name' value.");
  }
  if (!parent.second)
  {
    is_interface = true;
  }
  if (!gtype.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No`gtype-id'value.");
  }
  create_namespace (statement);
  object = new Api::Object (cname.first);
  if (!m_namespace->add_object (object))
  {
    delete object;
    object = m_namespace->get_object (cname.first);
    if (!object)
    {
      std::ostringstream oss;

      oss << "Failed to insert object `" << cname.first << "' into namespace `" << m_namespace->get_id () << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  object->set_parent (parent.first);
  object->set_gtype (gtype.first);
  object->set_is_interface (is_interface);
}

void DefsParser::on_function (const Statement& statement)
{
  const Statement::StringBoolPair cname (statement.get_value ("c-name"));
  const Statement::StringBoolPair ret_type (statement.get_value ("return-type"));
  Api::Function* function (0);

  if (!cname.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `c-name' value.");
  }
  if (!ret_type.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `return-type' value.");
  }
  create_namespace (statement);
  function = new Api::Function (cname.first);
  if (!m_namespace->add_function (function))
  {
    delete function;
    function = m_namespace->get_function (cname.first);
    if (!function)
    {
      std::ostringstream oss;

      oss << "Failed to insert function `" << cname.first << "' into namespace `" << m_namespace->get_id () << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  function->set_ret_type (ret_type.first);
  Statement::ConstIteratorBoolPair values_end (statement.get_list_end ("parameters"));
  if (!values_end.second)
  {
    return;
  }
  for (Statement::ConstIteratorBoolPair values_iter = statement.get_list_begin ("parameters"); values_iter.first != values_end.first; values_iter.first++)
  {
    const int elements_count (values_iter.first->size ());
    if (elements_count != 2)
    {
      std::ostringstream oss;

      oss << "Expected 2 values in `parameters' list element, got" << elements_count << ".";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    const std::string param_name ((*(values_iter.first))[0]);
    const std::string param_type ((*(values_iter.first))[1]);
    if (param_name.empty () || param_type.empty ())
    {
      std::ostringstream oss;

      oss << "Somehow empty string in statement placed in " << statement.get_file () << ", " << statement.get_line () << " slipped through earlier checks.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
    Api::Param* param = new Api::Param (param_name);
    if (!function->append_param (param))
    {
      delete param;
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, "Failed to insert parameter into function.");
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
  const Statement::StringBoolPair name (statement.get_value ("c-name"));
  Api::Enum* enumeration (0);

  if (!name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `c-name' value.");
  }
  create_namespace (statement);
  enumeration = new Api::Enum (name.first);
  if (!m_namespace->add_enum (enumeration))
  {
    delete enumeration;
    enumeration = 0;
    if (m_namespace->get_enum (name.first))
    {
      std::ostringstream oss;

      oss << (flags ? "Flags" : "Enum") << " `" << name.first << "' " << (flags ? "were" : "was") << "already defined.";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    else
    {
      std::ostringstream oss;

      oss << "Failed to add " << (flags ? "flags" : "enum") << " `" << name.first << "' into namespace `" << m_namespace->get_id ()<<"'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  enumeration->set_is_flags (flags);
  Statement::ConstIteratorBoolPair values_end (statement.get_list_end ("values"));
  if (!values_end.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `values' list specified in statement.");
  }
  for (Statement::ConstIteratorBoolPair values_iter = statement.get_list_begin ("values"); values_iter.first != values_end.first; values_iter.first++)
  {
    const int elements_count (values_iter.first->size ());
    if (elements_count != 3)
    {
      std::ostringstream oss;

      oss << "Expected 3 values in `values' list element, got " << elements_count << ".";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    const std::string elem_name ((*(values_iter.first))[1]);
    const std::string elem_value ((*(values_iter.first))[2]);
    if (elem_name.empty () || elem_value.empty ())
    {
      std::ostringstream oss;

      oss << "Somehow empty string in statement placed in " << statement.get_file () << ", " << statement.get_line () << " slipped through earlier checks.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
    Api::Enum::Element* element = new Api::Enum::Element (elem_name, elem_value);
    if (!enumeration->append_element (element))
    {
      std::ostringstream oss;

      delete element;
      oss << "Failed to insert element into " << (flags ? "flags" : "enum") << ".";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
}

void DefsParser::create_namespace (const Statement& statement)
{
  const std::string parameter (m_types_ns[statement.get_type ()]);
  const Statement::StringBoolPair name = statement.get_value (parameter);
  const std::string ns_name = Api::Namespace::get_namespace_name (name.first);

  if (!name.second)
  {
    return;
  }
  if (m_namespace)
  {
    const std::string m_ns_name (m_namespace->get_id ());
    if (m_ns_name != ns_name)
    {
      std::ostringstream oss;

      oss << "Namespace is already defined (`" << m_ns_name << "'), but got also other (`" << ns_name << "').";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str());
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
