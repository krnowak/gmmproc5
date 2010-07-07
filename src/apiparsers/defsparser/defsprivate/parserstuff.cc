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
#include <memory>

// defsprivate
#include "parserstuff.h"

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

namespace DefsPrivate
{

ParserStuff::ParserStuff (const std::string& path)
: m_tokens (),
  m_statements (),
  m_directory (dirname (path)),
  m_namespace (0),
  m_statement_actions (),
  m_parsed_files (std::deque<std::pair<std::string, std::string> > (1, std::make_pair (path, read_contents (path)))),
  m_types_ns ()
{
  m_statement_actions["include"] = std::bind (&ParserStuff::on_include, this, _1);
  m_statement_actions["define-flags-extended"] = std::bind (&ParserStuff::on_flags, this, _1);
  m_statement_actions["define-enum-extended"] = std::bind (&ParserStuff::on_enum, this, _1);
  m_statement_actions["define-object"] = std::bind (&ParserStuff::on_object, this, _1);
  m_statement_actions["define-function"] = std::bind (&ParserStuff::on_function, this, _1);
  m_statement_actions["define-method"] = std::bind (&ParserStuff::on_method, this, _1);
  m_statement_actions["define-property"] = std::bind (&ParserStuff::on_property, this, _1);
  m_statement_actions["define-signal"] = std::bind (&ParserStuff::on_signal, this, _1);
  m_statement_actions["define-vfunc"] = std::bind (&ParserStuff::on_vfunc, this, _1);
  m_statement_actions["define-flags"] = std::bind (&ParserStuff::on_omit, this, _1);
  m_statement_actions["define-enum"] = std::bind (&ParserStuff::on_omit, this, _1);
  
  m_types_ns["include"] = "";
  m_types_ns["define-flags-extended"] = "c-name";
  m_types_ns["define-enum-extended"] = "c-name";
  m_types_ns["define-object"] = "c-name";
  m_types_ns["define-function"] = "c-name";
  m_types_ns["define-method"] = "c-name";
  m_types_ns["define-property"] = "of-object";
  m_types_ns["define-signal"] = "of-object";
  m_types_ns["define-vfunc"] = "of-object";
  m_types_ns["define-flags"] = "";
  m_types_ns["define-enum"] = "";
}

ParserStuff::~ParserStuff ()
{}

Api::Namespace* ParserStuff::get_namespace ()
{
  
}

void ParserStuff::tokenize ()
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
    char c = *contents_iter;
    
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
            if (( c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-' || c == '_' || c == '#'))
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
          m_tokens.push_back (std::string (c));
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
            if (( c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '-' && c != '_' && c != '#'))
            {
              // syntax error
              return;
            }
          }
        }
        if (push)
        {
          m_tokens.push_back (std::string (token_begin, contents_iter);
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
  m_contents.clear();
}

void ParserStuff::statementize ()
{
  std::auto_ptr<StatementizeTask> statementize_task (new StatementizeTask (m_parsed_files.top().first));
  m_statements = statementize_task->statementize (m_tokens);
}

void ParserStuff::apicize ()
{
  std::list<Statement>::iterator statements_end (m_statements.end ());
  
  for (std::list<Statement>::iterator statement_iter (m_statements.begin ()); statement_iter != statements_end; statement_iter++)
  {
    const std::string type (statement_iter->get_type ());
    std::map<std::string, std::function<void (const Statement&)> >::iterator it (m_statement_actions.find (type));
    
    if (it != m_statement_actions.end ())
    {
      it->second();
    }
    else
    {
      //error
      return;
    }
  }
}

Api::Namespace* ParserStuff::parse_vfunc ()
{
  Api::Namespace* ns (0);
  
  parse ();
  ns = m_namespace;
  m_namespace = 0;
  return ns;
}

void ParserStuff::parse ()
{
  tokenize ();
  statementize ();
  apicize ();
}

std::string ParserStuff::read_contents (const std::string& filename)
{
  std::ifstream       file  (filename.c_str ());
  std::ostringstream  oss;
  
  if (file.good())
  {
    file >> oss.rdbuf ();
  }
  return oss.str ();
}

std::string ParserStuff::dirname (const std::string& filename)
{
  size_t found (filename.find_last_of ("/"));
  
  if (found == std::string::npos)
  {
    return std::string ("./");
  }
  return filename.substr (0, found + 1);
}

void ParserStuff::on_include (const Statement& statement)
{
  std::string path (m_directory + statement.get_header ())
  m_parsed_files.push (FileStack::value_type (path, read_contents (path)));
  parse ();
  m_parsed_files.pop ();
}

void ParserStuff::on_enum (const Statement& statement);
{
  create_ef( statement, false );
}

void ParserStuff::on_flags (const Statement& statement);
{
  create_ef( statement, true );
}

void ParserStuff::on_object (const Statement& statement);
{
  std::string cname (statement.get_value ("c-name"));
  std::string parent (statemnt.get_value ("parent"));
  std::string gtype (statement.get_value ("gtype-id"));
  Api::Object* object (0);
  
  if( cname.empty() || gtype.empty() || parent.empty() )
  {
    // error
    return;
  }
  create_namespace (statement);
  object = new Api::Object (cname);
  if (!m_namespace.add_object (object))
  {
    delete object;
    object = m_namespace.get_object (cname);
    if (!object)
    {
      // error
      return;
    }
  }
  object->set_parent (parent);
  object->set_gtype (gtype);
}

void ParserStuff::on_function (const Statement& statement);
{
  std::string cname (statement.get_value ("c-name"));
  std::string ret_val (statement.get_value ("return-type"));
  Api::Function* function (0);
  
  if (cname.empty () || retval.empty ())
  {
    // error
    return;
  }
  create_namespace (statement);
  function = new Api::Function (cname);
  if (!m_namespace.add_function (function))
  {
    delete function;
    function = m_namespace.get_function (cname);
    if (!function)
    {
      // error
      return;
    }
  }
  function->set_ret_val (ret_val);
  std::list<std::vector<std::string> >::const_iterator values_end (statement.get_list_end ("parameters"));
  for (std::list<std::vector<std::string> >::const_iterator values_iter = statement.get_list_begin ("parameters"); values_iter != values_end; values_iter++)
  {
    if (values_iter->length () != 2)
    {
      // error
      return;
    }
    std::string param_name ((*values_iter)[1]);
    std::string param_type ((*values_iter)[2]);
    if (param_name.empty () || param_type.empty ())
    {
      // error
      return;
    }
    Param* param = new Param (param_name);
    if (!function->append_param (param))
    {
      delete param;
      // error
      return;
    }
  }
}

void ParserStuff::on_method (const Statement& statement);
{

}

void ParserStuff::on_property (const Statement& statement);
{

}

void ParserStuff::on_signal (const Statement& statement);
{

}

void ParserStuff::on_vfunc (const Statement& statement);
{

}

void ParserStuff::on_omit (const Statement& statement);
{
  // empty
}
  
void ParserStuff::create_ef (const Statement& statement, bool flags)
{
  Api::Enum* enumeration ( 0 );
  std::string name ( statement.get_value ("c-name"));
  
  if ( name.empty() )
  {
    // error
    return;
  }
  create_namespace (statement);
  enumeration = new Api::Enum (name, flags);
  if ( !m_namespace.add_enum (enumeration))
  {
    delete enumeration;
    enumeration = 0;
    return;
  }
  std::list<std::vector<std::string> >::const_iterator values_end (statement.get_list_end ("values"));
  for (std::list<std::vector<std::string> >::const_iterator values_iter = statement.get_list_begin ("values"); values_iter != values_end; values_iter++)
  {
    if (values_iter->length () != 3)
    {
      // error
      return;
    }
    std::string elem_name ((*values_iter)[1]);
    std::string elem_value ((*values_iter)[2]);
    if (elem_name.empty () || elem_value.empty ())
    {
      // error
      return;
    }
    Api::Enum::Element* element = new Api::Enum::Element (, );
    if (!enumeration->append_element (element))
    {
      delete element;
      // error
      return;
    }
  }
}

void ParserStuff::create_namespace (const Statement& statement)
{
  std::string parameter (m_types_ns[statement.get_type ()]);
  std::string name = statement.get_value (parameter);
  std::string ns_name = Namespace::get_namespace_name (name);
  if (name.empty ())
  {
    return;
  }
  if (m_namespace)
  {
    if (m_namespace.get_id () != ns_name)
    {
      //error
    }
  }
  else
  {
    m_namespace = new Namespace (ns_name);
  }
}

} // namespace DefsPrivate

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc
