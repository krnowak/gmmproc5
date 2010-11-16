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
#include "utilities.h"

// api
#include "enum.h"
#include "function.h"
#include "namespace.h"
#include "object.h"
#include "param.h"
#include "property.h"
#include "signal.h"

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

struct DefsParser::DefsParserImpl
{
  typedef std::unordered_map<std::string, std::function<void (const Statement&)> > StringFunctionMap;
  typedef std::stack<std::pair<std::string, std::string> > FileStack;
  typedef std::unordered_map<std::string, std::string> StringStringMap;
  typedef std::stack<std::list<std::string> > TokensStack;
  typedef std::stack<std::list<Statement> > StatementsStack;
  typedef std::unordered_map<std::string, Api::RunFlags> StringRunFlagsMap;

  DefsParserImpl ();

  void tokenize ();
  void statementize ();
  void apicize ();
  void parse_round ();

  void on_include (const Statement& statement);
  void on_enum (const Statement& statement);
  void on_flags (const Statement& statement);
  void on_object (const Statement& statement);
  void on_function (const Statement& statement);
  void on_method (const Statement& statement);
  void on_property (const Statement& statement);
  void on_signal (const Statement& statement);
  void on_vfunc (const Statement& statement);
  void on_omit (const Statement& statement);

  void create_enum_or_flags (const Statement& statement, bool flags);
  void create_namespace (const Statement& statement);

  TokensStack m_tokens;
  StatementsStack m_statements;
  std::string m_directory;
  Api::NamespacePtr m_namespace;
  StringFunctionMap m_statement_actions;
  FileStack m_parsed_files;
  StringStringMap m_types_ns;
  StringRunFlagsMap m_runtypes;
};

DefsParser::DefsParserImpl::DefsParserImpl ()
: m_tokens (),
  m_statements (),
  m_directory (),
  m_namespace (),
  m_statement_actions (),
  m_parsed_files (),
  m_types_ns ()
{
  const std::string c_name ("c-name");
  const std::string of_object ("of-object");
  const std::string empty;

  m_statement_actions["include"] = std::bind (&DefsParser::DefsParserImpl::on_include, this, std::placeholders::_1);
  m_statement_actions["define-flags-extended"] = std::bind (&DefsParser::DefsParserImpl::on_flags, this, std::placeholders::_1);
  m_statement_actions["define-enum-extended"] = std::bind (&DefsParser::DefsParserImpl::on_enum, this, std::placeholders::_1);
  m_statement_actions["define-object"] = std::bind (&DefsParser::DefsParserImpl::on_object, this, std::placeholders::_1);
  m_statement_actions["define-function"] = std::bind (&DefsParser::DefsParserImpl::on_function, this, std::placeholders::_1);
  m_statement_actions["define-method"] = std::bind (&DefsParser::DefsParserImpl::on_method, this, std::placeholders::_1);
  m_statement_actions["define-property"] = std::bind (&DefsParser::DefsParserImpl::on_property, this, std::placeholders::_1);
  m_statement_actions["define-signal"] = std::bind (&DefsParser::DefsParserImpl::on_signal, this, std::placeholders::_1);
  m_statement_actions["define-vfunc"] = std::bind (&DefsParser::DefsParserImpl::on_vfunc, this, std::placeholders::_1);
  m_statement_actions["define-flags"] = std::bind (&DefsParser::DefsParserImpl::on_omit, this, std::placeholders::_1);
  m_statement_actions["define-enum"] = std::bind (&DefsParser::DefsParserImpl::on_omit, this, std::placeholders::_1);

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

  m_runtypes["first"] = Api::RUN_FIRST;
  m_runtypes["last"] = Api::RUN_LAST;
  m_runtypes["both"] = Api::RUN_FIRST | Api::RUN_LAST;
  m_runtypes["unknown"] = Api::RUN_UNKNOWN;
}

void DefsParser::DefsParserImpl::tokenize ()
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

  for (std::string::iterator contents_iter (token_begin); contents_iter != contents_end; ++contents_iter)
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
            ++current_line;
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
          --contents_iter;
        }
        break;
      }
      case TOKEN_COMMENT:
      {
        if (c == '\n')
        {
          token_type = TOKEN_NONE;
          --contents_iter;
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

void DefsParser::DefsParserImpl::statementize ()
{
  const std::string path (m_parsed_files.top ().first);
  std::shared_ptr<StatementizeTask> statementize_task (new StatementizeTask (path));

  m_statements.push (statementize_task->statementize (m_tokens.top ()));
}

void DefsParser::DefsParserImpl::apicize ()
{
  std::list<Statement>::iterator statements_end (m_statements.top ().end ());

  for (std::list<Statement>::iterator statement_iter (m_statements.top ().begin ()); statement_iter != statements_end; ++statement_iter)
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

void DefsParser::DefsParserImpl::parse_round ()
{
  tokenize ();
  statementize ();
  apicize ();
}

void DefsParser::DefsParserImpl::on_include (const Statement& statement)
{
  const std::string path (m_directory + statement.get_header ());

  m_parsed_files.push (FileStack::value_type (path, Proc::Common::Utilities::read_contents (path)));
  m_tokens.push (TokensStack::value_type ());
  // new statements are pushed in statementize method
  parse_round ();
  m_statements.pop ();
  m_tokens.pop ();
  m_parsed_files.pop ();
}

void DefsParser::DefsParserImpl::on_enum (const Statement& statement)
{
  create_enum_or_flags (statement, false);
}

void DefsParser::DefsParserImpl::on_flags (const Statement& statement)
{
  create_enum_or_flags (statement, true);
}

void DefsParser::DefsParserImpl::on_object (const Statement& statement)
{
  const Statement::StringBoolPair c_name (statement.get_value ("c-name"));

  if (!c_name.second)
  {
    throw SyntaxError (statement.get_file (),statement.get_line (), "No `c-name' value.");
  }

  const Statement::StringBoolPair parent (statement.get_value ("parent"));
  bool is_interface (false);

  if (!parent.second)
  {
    is_interface = true;
  }

  const Statement::StringBoolPair gtype (statement.get_value ("gtype-id"));

  if (!gtype.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No`gtype-id' value.");
  }
  create_namespace (statement);

  Api::ObjectPtr object (new Api::Object (c_name.first));

  if (!m_namespace->add_object (object))
  {
    std::ostringstream oss;

    if (m_namespace->get_object (c_name.first))
    {
      oss << "Object `" << c_name.first << "' was already defined.";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    else
    {
      oss << "Failed to insert object `" << c_name.first << "' into namespace `" << m_namespace->get_id () << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  object->set_parent (parent.first);
  object->set_gtype (gtype.first);
  object->set_is_interface (is_interface);
}

void DefsParser::DefsParserImpl::on_function (const Statement& statement)
{
  const Statement::StringBoolPair c_name (statement.get_value ("c-name"));

  if (!c_name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `c-name' value.");
  }

  const Statement::StringBoolPair ret_type (statement.get_value ("return-type"));

  if (!ret_type.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `return-type' value.");
  }
  create_namespace (statement);

  Api::FunctionPtr function (new Api::Function (c_name.first));

  if (!m_namespace->add_function (function))
  {
    std::ostringstream oss;

    if (m_namespace->get_function (c_name.first))
    {
      oss << "Function `" << c_name.first << "' was already defined.";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    else
    {
      oss << "Failed to insert function `" << c_name.first << "' into namespace `" << m_namespace->get_id () << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  function->set_ret_type (ret_type.first);

  Statement::ConstIteratorBoolPair values_end (statement.get_list_end ("parameters"));

  if (!values_end.second)
  {
    return;
  }
  for (Statement::ConstIteratorBoolPair values_iter (statement.get_list_begin ("parameters")); values_iter.first != values_end.first; ++values_iter.first)
  {
    const int elements_count (values_iter.first->size ());

    if (elements_count != 2)
    {
      std::ostringstream oss;

      oss << "Expected 2 values in `parameters' list element, got " << elements_count << ".";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }

    const std::string param_name ((*(values_iter.first))[0]);
    const std::string param_type ((*(values_iter.first))[1]);

    if (param_name.empty () || param_type.empty ())
    {
      std::ostringstream oss;

      oss << "Empty string in statement placed in " << statement.get_file () << ", " << statement.get_line () << ".";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }

    Api::ParamPtr param (new Api::Param (param_name));

    param->set_type (param_type);
    function->append_param (param);
  }

  Statement::BoolBoolPair varargs (statement.get_bool ("varargs"));

  if (varargs.second)
  {
    function->set_has_varargs (varargs.first);
  }
}

void DefsParser::DefsParserImpl::on_method (const Statement& statement)
{
  const Statement::StringBoolPair obj_name (statement.get_value ("of-object"));

  if (!obj_name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `of-object' value.");
  }

  Api::ObjectPtr object (m_namespace->get_object (obj_name.first));

  if (!object)
  {
    std::ostringstream oss;

    oss << "No object `" << obj_name.first << "' was defined in namespace `" << m_namespace->get_id () << "'.";
    throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
  }

  const Statement::StringBoolPair ret_type (statement.get_value ("return-type"));

  if (!ret_type.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `return-type' value.");
  }

  const Statement::StringBoolPair c_name (statement.get_value ("c-name"));

  if (!c_name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `c-name' value.");
  }

  Api::FunctionPtr method (new Api::Function (c_name.first));

  if (!object->add_method (method))
  {
    std::ostringstream oss;

    if (object->get_method (c_name.first))
    {
      oss << "Method `" << c_name.first << "' was already defined.";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    else
    {
      oss << "Failed to insert method `" << c_name.first << "' into object `" << obj_name.first << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  method->set_ret_type (ret_type.first);

  Statement::ConstIteratorBoolPair values_end (statement.get_list_end ("parameters"));

  if (!values_end.second)
  {
    return;
  }
  for (Statement::ConstIteratorBoolPair values_iter (statement.get_list_begin ("parameters")); values_iter.first != values_end.first; ++values_iter.first)
  {
    const int elements_count (values_iter.first->size ());

    if (elements_count != 2)
    {
      std::ostringstream oss;

      oss << "Expected 2 values in `parameters' list element, got " << elements_count << ".";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }

    const std::string param_name ((*(values_iter.first))[0]);
    const std::string param_type ((*(values_iter.first))[1]);

    if (param_name.empty () || param_type.empty ())
    {
      std::ostringstream oss;

      oss << "Empty string in statement placed in " << statement.get_file () << ", " << statement.get_line () << ".";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }

    Api::ParamPtr param (new Api::Param (param_name));

    param->set_type (param_type);
    method->append_param (param);
  }

  Statement::BoolBoolPair varargs (statement.get_bool ("varargs"));

  if (varargs.second)
  {
    method->set_has_varargs (varargs.first);
  }
}

void DefsParser::DefsParserImpl::on_property (const Statement& statement)
{
  const Statement::StringBoolPair obj_name (statement.get_value ("of-object"));

  if (!obj_name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `of-object' value.");
  }

  Api::ObjectPtr object (m_namespace->get_object (obj_name.first));

  if (!object)
  {
    std::ostringstream oss;

    oss << "No object `" << obj_name.first << "' was defined in namespace `" << m_namespace->get_id () << "'.";
    throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
  }

  const Statement::StringBoolPair prop_type (statement.get_value ("prop-type"));

  if (!prop_type.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `prop-type' value.");
  }

  const std::string name (statement.get_header ());

  if (name.empty ())
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "Empty header.");
  }

  Api::PropertyPtr property (new Api::Property (name));

  if (!object->add_property (property))
  {
    std::ostringstream oss;

    if (object->get_property (name))
    {
      oss << "Property `" << name << "' was already defined.";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    else
    {
      oss << "Failed to insert property `" << name << "' into object `" << obj_name.first << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  property->set_type (prop_type.first);

  Api::PropertyFlags flags (Api::PROPERTY_INVALID);
  Statement::BoolBoolPair readable (statement.get_bool ("readable"));

  if (!readable.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `readable' bool value.");
  }
  else if (readable.first)
  {
    flags |= Api::PROPERTY_READABLE;
  }

  Statement::BoolBoolPair writable (statement.get_bool ("writable"));

  if (!writable.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `writable' bool value.");
  }
  else if (writable.first)
  {
    flags |= Api::PROPERTY_WRITABLE;
  }

  Statement::BoolBoolPair construct_only (statement.get_bool ("construct-only"));

  if (!construct_only.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `construct-only' bool value.");
  }
  else if (construct_only.first)
  {
    flags |= Api::PROPERTY_CONSTRUCT_ONLY;
  }
  property->set_flags (flags);
}

void DefsParser::DefsParserImpl::on_signal (const Statement& statement)
{
  const Statement::StringBoolPair obj_name (statement.get_value ("of-object"));

  if (!obj_name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `of-object' value.");
  }

  Api::ObjectPtr object (m_namespace->get_object (obj_name.first));

  if (!object)
  {
    std::ostringstream oss;

    oss << "No object `" << obj_name.first << "' was defined in namespace `" << m_namespace->get_id () << "'.";
    throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
  }

  const Statement::StringBoolPair ret_type (statement.get_value ("return-type"));

  if (!ret_type.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `return-type' value.");
  }

  const std::string name (statement.get_header ());

  if (name.empty ())
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "Empty header.");
  }

  Api::SignalPtr signal (new Api::Signal (name));

  if (!object->add_signal (signal))
  {
    std::ostringstream oss;

    if (object->get_signal (name))
    {
      oss << "Signal `" << name << "' was already defined.";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    else
    {
      oss << "Failed to insert signal `" << name << "' into object `" << obj_name.first << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  signal->set_ret_type (ret_type.first);

  Statement::StringBoolPair when (statement.get_value ("when"));

  if (!when.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `when' value.");
  }

  StringRunFlagsMap::const_iterator it (m_runtypes.find (when.first));

  if (it == m_runtypes.end ())
  {
    std::ostringstream oss;

    oss << "Invalid signal run flag `" << when.first << "'.";
    throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
  }
  signal->set_run_flags (m_runtypes[when.first]);

  Statement::ConstIteratorBoolPair values_end (statement.get_list_end ("parameters"));

  if (!values_end.second)
  {
    return;
  }
  for (Statement::ConstIteratorBoolPair values_iter (statement.get_list_begin ("parameters")); values_iter.first != values_end.first; ++values_iter.first)
  {
    const int elements_count (values_iter.first->size ());

    if (elements_count != 2)
    {
      std::ostringstream oss;

      oss << "Expected 2 values in `parameters' list element, got " << elements_count << ".";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }

    const std::string param_name ((*(values_iter.first))[0]);
    const std::string param_type ((*(values_iter.first))[1]);

    if (param_name.empty () || param_type.empty ())
    {
      std::ostringstream oss;

      oss << "Empty string in statement placed in " << statement.get_file () << ", " << statement.get_line () << ".";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }

    Api::ParamPtr param (new Api::Param (param_name));

    param->set_type (param_type);
    signal->append_param (param);
  }
}

void DefsParser::DefsParserImpl::on_vfunc (const Statement& statement)
{
  const Statement::StringBoolPair obj_name (statement.get_value ("of-object"));

  if (!obj_name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `of-object' value.");
  }

  Api::ObjectPtr object (m_namespace->get_object (obj_name.first));

  if (!object)
  {
    std::ostringstream oss;

    oss << "No object `" << obj_name.first << "' was defined in namespace `" << m_namespace->get_id () << "'.";
    throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
  }

  const Statement::StringBoolPair ret_type (statement.get_value ("return-type"));

  if (!ret_type.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `return-type' value.");
  }

  const std::string name (statement.get_header ());

  if (!name.empty ())
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "Empty header.");
  }

  Api::FunctionPtr vfunc (new Api::Function (name));

  if (!object->add_vfunc (vfunc))
  {
    std::ostringstream oss;

    if (object->get_vfunc (name))
    {
      oss << "Method `" << name << "' was already defined.";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }
    else
    {
      oss << "Failed to insert method `" << name << "' into object `" << obj_name.first << "'.";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }
  }
  vfunc->set_ret_type (ret_type.first);

  Statement::ConstIteratorBoolPair values_end (statement.get_list_end ("parameters"));

  if (!values_end.second)
  {
    return;
  }
  for (Statement::ConstIteratorBoolPair values_iter (statement.get_list_begin ("parameters")); values_iter.first != values_end.first; ++values_iter.first)
  {
    const int elements_count (values_iter.first->size ());

    if (elements_count != 2)
    {
      std::ostringstream oss;

      oss << "Expected 2 values in `parameters' list element, got " << elements_count << ".";
      throw SyntaxError (statement.get_file (), statement.get_line (), oss.str ());
    }

    const std::string param_name ((*(values_iter.first))[0]);
    const std::string param_type ((*(values_iter.first))[1]);

    if (param_name.empty () || param_type.empty ())
    {
      std::ostringstream oss;

      oss << "Empty string in statement placed in " << statement.get_file () << ", " << statement.get_line () << ".";
      throw Common::InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, oss.str ());
    }

    Api::ParamPtr param (new Api::Param (param_name));

    param->set_type (param_type);
    vfunc->append_param (param);
  }

  Statement::BoolBoolPair varargs (statement.get_bool ("varargs"));

  if (varargs.second)
  {
    vfunc->set_has_varargs (varargs.first);
  }
}

void DefsParser::DefsParserImpl::on_omit (const Statement& /* statement */)
{
  // empty
}

void DefsParser::DefsParserImpl::create_enum_or_flags (const Statement& statement, bool flags)
{
  const Statement::StringBoolPair name (statement.get_value ("c-name"));

  if (!name.second)
  {
    throw SyntaxError (statement.get_file (), statement.get_line (), "No `c-name' value.");
  }
  create_namespace (statement);

  Api::EnumPtr enumeration (new Api::Enum (name.first));

  if (!m_namespace->add_enum (enumeration))
  {
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
  for (Statement::ConstIteratorBoolPair values_iter (statement.get_list_begin ("values")); values_iter.first != values_end.first; ++values_iter.first)
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

    Api::Enum::ElementPtr element (new Api::Enum::Element (elem_name, elem_value));

    enumeration->append_element (element);
  }
}

void DefsParser::DefsParserImpl::create_namespace (const Statement& statement)
{
  const std::string parameter (m_types_ns[statement.get_type ()]);
  const Statement::StringBoolPair name (statement.get_value (parameter));
  const std::string ns_name (Api::Namespace::get_namespace_name (name.first));

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
    m_namespace = Api::NamespacePtr (new Api::Namespace (ns_name));
  }
}

DefsParser::DefsParser ()
: ApiParser (),
  m_pimpl (new DefsParserImpl)
{}

DefsParser::~DefsParser ()
{}

void DefsParser::set_file_to_parse_vfunc (const std::string& file_path)
{
  m_pimpl->m_parsed_files.push (DefsParserImpl::FileStack::value_type (file_path, Proc::Common::Utilities::read_contents (file_path)));
  m_pimpl->m_tokens.push (DefsParserImpl::TokensStack::value_type ());
  m_pimpl->m_statements.push (DefsParserImpl::StatementsStack::value_type ());
  m_pimpl->m_directory = Proc::Common::Utilities::dir_name (file_path);
}

void DefsParser::set_string_to_parse_vfunc (const std::string& contents)
{
  m_pimpl->m_parsed_files.push (DefsParserImpl::FileStack::value_type ("./memory", contents));
  m_pimpl->m_tokens.push (DefsParserImpl::TokensStack::value_type ());
  m_pimpl->m_statements.push (DefsParserImpl::StatementsStack::value_type ());
  m_pimpl->m_directory = Proc::Common::Utilities::dir_name ("./");
}

Api::NamespacePtr DefsParser::parse_vfunc ()
{
  Api::NamespacePtr ns;

  m_pimpl->parse_round ();
  m_pimpl->m_namespace.swap (ns);
  return ns;
}

std::string DefsParser::get_ext_vfunc () const
{
  return std::string ("defs");
}

void DefsParser::reset_vfunc ()
{
  {
    DefsParserImpl::TokensStack empty;

    m_pimpl->m_tokens.swap (empty);
  }
  {
    DefsParserImpl::StatementsStack empty;

    m_pimpl->m_statements.swap (empty);
  }
  {
    std::string empty;

    m_pimpl->m_directory.swap (empty);
  }
  {
    Api::NamespacePtr empty;

    m_pimpl->m_namespace.swap (empty);
  }
  {
    DefsParserImpl::FileStack empty;

    m_pimpl->m_parsed_files.swap (empty);
  }
  
}

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc
