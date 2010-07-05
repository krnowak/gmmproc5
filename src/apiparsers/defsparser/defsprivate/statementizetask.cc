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

// defsprivate
#include "statementizetask.h"

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

namespace DefsPrivate
{

StatementizeTask::StatementizeTask(const std::string& file)
: m_current_statement (),
  m_current_context (CONTEXT_OUTSIDE),
  m_long_token_num (0),
  m_list_elements_count (0),
  m_value (),
  m_key (),
  m_element_list (),
  m_elements (),
  m_token_actions (),
  m_current_line (1),
  m_statements (),
  m_file (file)
{
  m_token_actions["\n"] = std::bind (&StatementizeTask::on_token_newline, this);
  m_token_actions["("] = std::bind (&StatementizeTask::on_token_open_paren, this);
  m_token_actions[")"] = std::bind (&StatementizeTask::on_token_close_paren, this);
  m_token_actions["'"] = std::bind (&StatementizeTask::on_token_apostrophe, this);
}

std::list<Statement> StatementizeTask::statementize(const std::list<const std::string>& tokens)
{
  std::list<std::string>::const_iterator tokens_end (tokens.end ());

  cleanup();
  for (std::list<std::string>::const_iterator token_iter (tokens.begin ()); token_iter != tokens_end; token_iter++)
  {
    std::map<std::string, std::function< void()> >::iterator it (m_token_actions.find (*token_iter));

    if (it != m_token_actions.end ())
    {
      it->second();
    }
    else
    {
      on_token_other (*token_iter);
    }
  }
  return m_statements;
}
  
void StatementizeTask::on_token_newline ()
{
  m_current_line++;
}

void StatementizeTask::on_token_open_paren ()
{
  switch (m_current_context)
  {
    case CONTEXT_OUTSIDE:
    {
      m_current_context = CONTEXT_HEADER;
      m_current_statement.get_line() = m_current_line;
      m_current_statement.get_file() = m_file;
      break;
    }
    case CONTEXT_HEADER:
    {
      m_current_statement.get_type() = m_key;
      m_current_statement.get_header() = m_value;
      m_current_context = CONTEXT_LEVEL2;
      break;
    }
    case CONTEXT_LEVEL1:
    {
      m_current_context = CONTEXT_LEVEL2;
      break;
    }
    case CONTEXT_LEVEL2:
    case CONTEXT_BEFORE_LIST_ELEMENTS:
    {
      m_current_context = CONTEXT_LIST_ELEMENTS;
      break;
    }
    default:
    {
      //internal error
      return;
    }
  }
  m_long_token_num = 0;
}

void StatementizeTask::on_token_close_paren ()
{
  switch (m_current_context)
  {
    case CONTEXT_HEADER:
    case CONTEXT_LEVEL1:
    {
      m_current_context = CONTEXT_OUTSIDE;
      m_statements.push_back (m_current_statement);
      m_current_statement.clear ();
      break;
    }
    case CONTEXT_LEVEL2:
    {
      bool val_empty (m_value.empty ());
      bool list_empty (m_element_list.empty ());
      
      m_current_context = CONTEXT_LEVEL1;
      if (!(val_empty ^ list_empty))
      {
        // syntax error
        return;
      }
      else if (!val_empty)
      {
        switch (m_value[0])
        {
          case '#':
          {
            bool option (false);
            if (m_value.length () < 1)
            {
              // error
              return;
            }
            
            switch (m_value[1])
            {
              case 't':
              {
                option = true;
                break;
              }
              case 'f':
              {
                option = false;
                break;
              }
              default:
              {
                //error
                return;
              }
            }
            m_current_statement.get_bools ()[m_key] = option;
            break;
          }
          case '"':
          {
            size_t val_length (m_value.length ());
            
            if (val_length < 3)
            {
              // empty string error
              return;
            }
            m_current_statement.get_values ()[m_key] = m_value.substr (1, val_length - 2);
            break;
          }
          default:
          {
            // syntax error
            return;
          }
        }
      }
      else if (!list_empty)
      {
        m_current_statement.get_lists ()[m_key] = m_element_list;
      }
      else
      {
        // some kind of error
        return;
      }
      m_list_elements_count = 0;
      m_element_list.clear ();
      m_value.clear ();
      m_key.clear ();
      break;
    }
    case CONTEXT_LIST_ELEMENTS:
    {
      m_current_context = CONTEXT_LEVEL2;
      if (!m_list_elements_count)
      {
        m_list_elements_count = m_long_token_num;
      }
      else if (m_list_elements_count != m_long_token_num)
      {
        //error - different count of elements
        return;
      }
      m_element_list.push_back (m_elements);
      m_elements.clear ();
      m_elements.reserve (m_list_elements_count);
      break;
    }
    default:
    {
      //internal error
      return;
    }
  }
  m_long_token_num = 0;
}

void StatementizeTask::on_token_apostrophe ()
{
  if (m_current_context == CONTEXT_LEVEL2)
  {
    m_current_context = CONTEXT_BEFORE_LIST_ELEMENTS;
  }
  else
  {
    //syntax error
  }
}

void StatementizeTask::on_token_other (const std::string& token)
{
  switch (m_current_context)
  {
    case CONTEXT_LEVEL1:
    case CONTEXT_OUTSIDE:
    case CONTEXT_BEFORE_LIST_ELEMENTS:
    {
      // syntax error
      return;
    }
    case CONTEXT_HEADER:
    case CONTEXT_LEVEL2:
    {
      switch (m_long_token_num)
      {
        case 0:
        {
          m_key = token;
          break;
        }
        case 1:
        {
          m_value = token;
          break;
        }
        default:
        {
          //syntax error
          return;
        }
      }
      m_long_token_num++;
      break;
    }
    case CONTEXT_LIST_ELEMENTS:
    {
      m_elements.push_back (token);
      m_long_token_num++;
      break;
    }
    default:
    {
      //internal error
      return;
    }
  }
}

void StatementizeTask::cleanup ()
{
  m_current_statement.clear ();
  m_current_context = CONTEXT_OUTSIDE;
  m_long_token_num = 0;
  m_list_elements_count = 0;
  m_value.clear ();
  m_key.clear ();
  m_element_list.clear ();
  m_elements.clear ();
  m_current_line = 1;
  m_statements.clear ();
  m_file.clear();
}

} // namespace DefsPrivate

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc
