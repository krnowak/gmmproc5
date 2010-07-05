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
#include <iostream>
#include <sstream>
//#include <stdexcept>

// defsparser
#include "defsparser.h"
#include "parserstuff.h"
#include "statement.h"

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

namespace DefsPrivate
{

static std::string read_contents( const std::string& filename );

static std::string dirname( const std::string& filename );

static std::list< std::string > tokenize (const std::string& str);

} // namespace DefsPrivate

DefsParser::DefsParser (const std::string& filename)
: m_stuff (0)
/*  m_directory (dirname (filename)),
  m_contents (read_contents (filename)),
  m_namespace (0),
  m_token_types_actions ()*/
{
  m_token_types_actions[ "include" ] = std::make_pair( INCLUDE, &Proc::Parsers::ApiParsers::DefsParser::on_include );
  m_token_types_actions[ "define-flags-extended" ] = std::make_pair( FLAGS, &Proc::Parsers::ApiParsers::DefsParser::on_flags );
  m_token_types_actions[ "define-enum-extended" ] = std::make_pair( ENUM, &Proc::Parsers::ApiParsers::DefsParser::on_enum );
  m_token_types_actions[ "define-object" ] = std::make_pair( OBJECT, &Proc::Parsers::ApiParsers::DefsParser::on_object );
  m_token_types_actions[ "define-function" ] = std::make_pair( FUNCTION, &Proc::Parsers::ApiParsers::DefsParser::on_function );
  m_token_types_actions[ "define-method" ] = std::make_pair( METHOD, &Proc::Parsers::ApiParsers::DefsParser::on_method );
  m_token_types_actions[ "define-property" ] = std::make_pair( PROPERTY, &Proc::Parsers::ApiParsers::DefsParser::on_property );
  m_token_types_actions[ "define-signal" ] = std::make_pair( SIGNAL, &Proc::Parsers::ApiParsers::DefsParser::on_signal );
  m_token_types_actions[ "define-vfunc" ] = std::make_pair( VFUNC, &Proc::Parsers::ApiParsers::DefsParser::on_vfunc );
  m_token_types_actions[ "define-flags" ] = std::make_pair( OMIT, &Proc::Parsers::ApiParsers::DefsParser::on_omit );
  m_token_types_actions[ "define-enum" ] = std::make_pair( OMIT, &Proc::Parsers::ApiParsers::DefsParser::on_omit );
}

DefsParser::~DefsParser()
{}

Api::Namespace* DefsParser::parse_vfunc()
{
  std::list<std::string> tokens (tokenize (m_contents));
  std::list<std::string>::iterator tokens_end (tokens.end ());
  unsigned int current_line (0);
  enum Context
  {
    CONTEXT_OUTSIDE,
    CONTEXT_HEADER,
    CONTEXT_LEVEL1,
    CONTEXT_LEVEL2,
    CONTEXT_LIST_ELEMENT
  } current_context (CONTEXT_OUTSIDE);
  DefsPrivate::Statement statement;
  int long_token_num (0);
  int list_elements_count (0);
  std::string value;
  std::string key;
  std::list<std::vector<std::string> > element_list;
  std::vector<std::string> elements;
  
  for (std::list<std::string>::iterator token_iter (tokens.begin ()); token_iter != tokens_end; token_iter++)
  {
    size_t token_length (token_iter->length());
    
    if (*iter_token == '\n')
    {
      current_line++;
    }
    else if ()
    
    if (token_length == 1)
    {
      switch ((*token_iter)[0])
      {
        case '\n':
        {
          current_line++;
          break;
        }
        case '(':
        {
          switch (current_context)
          {
            case CONTEXT_OUTSIDE:
            {
              current_context = CONTEXT_HEADER;
              break;
            }
            case CONTEXT_HEADER:
            {
              statement.get_type() = key;
              statement.get_header() = value;
              current_context = CONTEXT_LEVEL2;
              break;
            }
            case CONTEXT_LEVEL1:
            {
              current_context = CONTEXT_LEVEL2;
              break;
            }
            case CONTEXT_LEVEL2:
            {
              current_context = CONTEXT_LIST_ELEMENT;
              break;
            }
            default:
            {
              //internal error
              return 0;
            }
          }
          long_token_num = 0;
          break;
        }
        case ')':
        {
          switch (current_context)
          {
            case CONTEXT_HEADER:
            case CONTEXT_LEVEL1:
            {
              current_context = CONTEXT_OUTSIDE;
              parse_statement (statement);
              statement.clear();
              break;
            }
            case CONTEXT_LEVEL2:
            {
              bool val_empty (value.empty ());
              bool list_empty (element_list.empty ());
              current_context = CONTEXT_LEVEL1;
              if (val_empty ^ list_empty) 
              {
                // syntax error
                return 0;
              }
              else if (!val_empty)
              {
                switch (value[0])
                {
                  case '#':
                  {
                    statement.get_bools()[key] = (value[1] == 't');
                    break;
                  }
                  case '"':
                  {
                    statement.get_values()[key] = value.substr(1, value.length() - 2);
                    break;
                  }
                  default:
                  {
                    // syntax error
                    return 0;
                  }
                }
              }
              else if (!list_empty)
              {
                statement.get_lists()[key] = element_list;
              }
              else
              {
                // some kind of error
                return 0;
              }
              list_elements_count = 0;
              element_list.clear ();
              value.clear ();
              key.clear ();
              break;
            }
            case CONTEXT_LIST_ELEMENT:
            {
              current_context = CONTEXT_LEVEL2;
              if (!list_elements_count)
              {
                list_elements_count = long_token_num;
              }
              else if (list_elements_count != long_token_num)
              {
                //error - different count of elements
                return 0;
              }
              element_list.push_back (elements);
              elements.clear ();
              elements.reserve (list_elements_count);
              break;
            }
            default:
            {
              //internal error
              return 0;
            }
          }
          long_token_num = 0;
          break;
        }
        case '\'':
        {
          break;
        }
        default:
        {
          // syntax error
          return 0;
        }
      }
    }
    else if (token_length > 1)
    {
      switch (current_context)
      {
        case CONTEXT_OUTSIDE:
        {
          // syntax error
          return 0;
        }
        case CONTEXT_HEADER:
        {
          switch (long_token_num)
          {
            case 0:
            {
              key = *token_iter;
              break;
            }
            case 1:
            {
              value = *token_iter;
              break;
            }
            default:
            {
              //syntax error
              return 0;
            }
          }
          long_token_num++;
          break;
        }
        case CONTEXT_LEVEL1:
        {
          //syntax error
          return 0;
        }
        case CONTEXT_LEVEL2:
        {
          switch (long_token_num)
          {
            case 0:
            {
              key = *token_iter;
              break;
            }
            case 1:
            {
              value = *token_iter;
              break;
            }
            default:
            {
              //syntax error
              return 0;
            }
          }
          long_token_num++;
          break;
        }
        case CONTEXT_LIST_ELEMENT:
        {
          elements.push_back (*token_iter);
          long_token_num++;
          break;
        }
        default:
        {
          //internal error
          return 0;
        }
      }
    }
    else
    {
      // token error
      return 0;
    }
  }
}

std::string DefsParser::get_ext_vfunc () const
{
  return std::string ("defs");
}

void DefsParser::parse_statement( const DefsPrivate::Statement& statement )
{
  /*size_t      space_idx ( 0 );
  std::string copy      ( statement );
  std::string type;*/
  std::map< std::string, std::pair< DefineType, void (*)( const std::string& statement ) > >::iterator it;
  
  /*copy.erase( copy.find_last_of( ")" ) );
  copy.erase( 0, copy.find_first_of( "(" ) + 1 );
  space_idx = copy.find_first_of( " " );
  type = copy.substr( 0, space_idx );*/
  it = m_token_types_actions.find( statement.get_type() );
  if ( it != m_token_types_actions.end() )
  {
    if ( !m_namespace )
    {
      create_namespace( it->first, statement );
    }
    it->second( statement );
  }
  else
  {
    //std::cerr << "unknown token: " << statement << "\n";
	// syntax error
	return;
  }
}

void DefsParser::on_include( const std::string& statement )
{
  /*size_t bidx( statement.find_first_of( " ", statement.find( "i" ) ) + 1 );
  const std::string filename( statement.substr( bidx ) );
  const std::string rel_filename( m_directory + filename );
  std::istringstream iss( read_contents( rel_filename ) );
  
  parse_stream( iss );*/
}

void DefsParser::on_enum( const std::string& statement )
{
  //create_ef( statement, false );
}

void DefsParser::on_flags( const std::string& statement )
{
  //create_ef( statement, true );
}

void DefsParser::on_object( const std::string& statement )
{
  /*std::string cname( extract_value( statement, "c-name" ) );
  std::string parent( extract_value( statement, "parent" ) );
  std::string gtype( extract_value( statement, "gtype-id" ) );
  Object* obj( 0 );
  
  if( cname.empty() || gtype.empty() || parent.empty() )
  {
    return;
  }
  obj = new Object( cname, parent, gtype );
  if( !m_namespace.add_object( obj ) )
  {
    delete obj;
    obj = m_namespace.get_object( cname );
    if( !obj )
    {
      return;
    }
    if( !obj->get_parent().empty() || !obj->get_gtype().empty() )
    {
      return;
    }
    obj->set_parent( parent );
    obj->set_gtype( gtype );
  }*/
}

void DefsParser::on_function( const std::string& statement )
{
  /*std::string cname( extract_value( statement, "c-name" ) );
  std::string retval( extract_value( statement, "return-type" ) );
  
  if( cname.empty() || retval.empty() )
  {
    return;
  }*/
}

void DefsParser::on_method( const std::string& statement )
{

}

void DefsParser::on_property( const std::string& statement )
{

}

void DefsParser::on_signal( const std::string& statement )
{

}

void DefsParser::on_vfunc( const std::string& statement )
{

}

void DefsParser::on_omit( const std::string& statement )
{
  // empty
}

namespace DefsPrivate
{

static std::string read_contents (const std::string& filename)
{
  std::ifstream       file  (filename.c_str ());
  std::ostringstream  oss;
  
  if (file.good())
  {
    file >> oss.rdbuf ();
  }
  return oss.str ();
}

static std::string dirname (const std::string& filename)
{
  size_t found (filename.find_last_of ("/"));
  
  if (found == std::string::npos)
  {
    return std::string ("./");
  }
  return filename.substr (0, found + 1);
}

static std::list< std::string > tokenize (const std::string& str)
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
  std::string::iterator stre (str.end ());
  std::string::iterator tokb (str.begin ());
  int current_line (0);
  
  for (std::string::iterator stri (str.begin ()); stri != stre; stri++)
  {
    char c = *stri;
    
    switch (token_type)
    {
      case TOKEN_NONE:
      {
        bool push (false);
        
        if (c == '(' || c == ')' || c == '\'')
        {
          push = true;
        }
        else if (c == '\n')
        {
          push = true;
          current_line++;
        }
        else if (c == ';')
        {
          token_type = TOKEN_COMMENT;
        }
        else if (c == '"')
        {
          token_type = TOKEN_DQ;
          tokb = stri;
        }
        else if (( c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-' || c == '_' || c == '#'))
        {
          token_type = TOKEN_STR;
          tokb = stri;
        }
        else if ((c != ' ') && (c != '\t'))
        {
          // syntax error
          return tokens;
        }
        
        if (push)
        {
          tokens.push_back (std::string (c));
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
              tokens.push_back (std::string (tokb, stri + 1));
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
            return tokens;
          }
          default:
          {
            escape = false;
            break;
          }
        }
        break;
      }
      case TOKEN_STR:
      {
        bool push (false);
        bool decrement (false);
        
        if (c == '(' || c == ')' || c == '\'' || c == '\n' || c == ';' || c == '"')
        {
          push = decrement = true;
        }
        else if (c == ' ' || c == '\t')
        {
          push = true;
        }
        else if (( c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '-' && c != '_' && c != '#'))
        {
          // syntax error
          return tokens;
        }
        if (push)
        {
          tokens.push_back (std::string (tokb, stri);
          token_type = TOKEN_NONE;
        }
        if (decrement)
        {
          stri--;
        }
        break;
      }
      case TOKEN_COMMENT:
      {
        if (c == '\n')
        {
          token_type = TOKEN_NONE;
          stri--;
        }
        break;
      }
      default:
      {
        // internal error
        return tokens;
      }
    }
  }
  return tokens;
}

} // namespace DefsPrivate

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc









// OLD CRUFT!







/*
void DefsParser::parse_stream( std::istream& stream )
{
  while( stream.good() )
  {
    DefsPrivate::Statement statement (get_statement (stream));
    
    if ( !statement.empty() )
    {
      parse_statement( statement );
    }
  }
}
*/
/*
DefsPrivate::Statement DefsParser::get_statement( std::istream& stream )
{
  char c ( '\0' );
  
  do
  {
    c = stream.get();
    if ( c == ';' )
    {
      stream.ignore( 1024, '\n' );
    }
  }
  while( c != '(' && stream.good() );
  if( c != '(' )
  {
    return std::string();
  }
  stream.unget();
  return extract_parentheses( stream );
}
*/
/*
DefsPrivate::Statement DefsParser::get_statement (std::istream& stream)
{
  char        c                     ('\0');
  bool        inside_quotes         (false);
  bool        last_white            (false);
  bool        last_escape           (false);
  bool        inside_comment        (false);
  int         level                 (0);
  bool        inside_statement      (false);
  DefsPrivate::Statement statement;
  
  while (stream.good() && ((level > 0) || (!inside_statement)))
  {
    c = stream.get();
    if (inside_comment)
    {
      if (c == '\n')
      {
        inside_comment = false;
      }
    }
    else if (c == '\\')
    {
      last_escape = !last_escape;
    }
    else if (c == '"')
    {
      if (!last_escape)
      {
        inside_quotes = !inside_quotes;
      }
    }
    else if (c == '(')
    {
      inside_statement = true;
      level++;
      switch (level)
      {
        case 1:
        {
          break;
        }
        case 2:
        {
          break;
        }
        case 3:
        {
          break;
        }
      }
    }
  }
  
  
  
  //do
  //{
  //  c = stream.get();
  //  if ( c == ';' )
  //  {
  //    stream.ignore( 1024, '\n' );
  //  }
  //}
  //while( c != '(' && stream.good() );
  //if( c != '(' )
  //{
  //  return std::string();
  //}
  //statement.append( 1, c );
  while (stream.good() && level)
  {
    c = stream.get();
    if (comment)
    {
      if (c == '\n')
      {
        comment = false;
      }
    }
    else if (c == '\\')
    {
      last_white = false;
      last_escape = !last_escape;
    }
    else if (c == '"')
    {
      last_white = false;
      if( !last_escape )
      {
        inside_dq = !inside_dq;
      }
    }
    else if (!inside_dq)
    {
      if( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
      {
        if ( !last_white )
        {
          c = ' ';
          last_white = true;
        }
        else
        {
          continue;
        }
      }
      else if (c == ';')
      {
        comment = true;
      }
      else
      {
        last_white = false;
        if( c == '(' )
        {
          level++;
        }
        else if( c == ')' )
        {
          level--;
        }
      }
    }
    statement.append( 1, c );
  }
  return statement;
}
*/

/*void DefsParser::create_namespace( DefineType type, const DefsPrivate::Statement& statement )
{
  std::string parameter;
  std::string name;
  size_t bidx ( std::string::npos );
  size_t eidx ( bidx );
  switch( type )
  {
    case FUNCTION:
    case METHOD:
    case ENUM:
    case FLAGS:
    case OBJECT:
    {
      parameter = "c-type";
      break;
    }
    case PROPERTY:
    case SIGNAL:
    case VFUNC:
    {
      parameter = "of-object";
      break;
    }
    default:
    {
      return;
    }
  }
  name = statement.get_values()[parameter];
  //name = extract_value( statement, parameter );
  if ( name.empty() )
  {
    return;
  }
  m_namespace = new Namespace( Namespace::get_namespace_name( name ) );
}*/

/*void DefsParser::create_ef( const std::string& statement, bool flags )
{
  size_t bidx ( std::string::npos );
  size_t eidx ( std::string::npos );
  Enum* enumeration ( 0 );
  std::istringstream iss;
  std::string name ( extract_value( statement, "c-name" ) )
  
  if ( name.empty() )
  {
    return;
  }
  enumeration = new Enum( statement.substr( bidx, eidx - bidx ), flags );
  if ( !m_namespace.add_enum( enumeration ) )
  {
    delete enumeration;
    enumeration = 0;
    return;
  }
  bidx = statement.find( "values", eidx );
  if ( bidx == std::string::npos )
  {
    return;
  }
  iss = std::istringstream( statement.substr( bidx ) );
  while ( true )
  {
    std::string values( extract_parentheses( iss ) );
    std::pair< std::string, std::string > element;
    
    if ( values.empty() )
    {
      break;
    }
    element = get_enum_element( values );
    if ( element == std::pair< std::string, std::string >() )
    {
      break;
    }
    enumeration->push_back( element );
  }
}*/

/*std::string DefsParser::extract_parentheses( std::istream& stream )
{
  char        c           ( '\0' );
  bool        inside_dq   ( false );
  bool        last_white  ( false );
  bool        last_escape ( false );
  int         level       ( 1 );
  std::string statement;
  
  do
  {
    c = stream.get();
  }
  while( c != '(' && stream.good() );
  if( c != '(' )
  {
    return std::string();
  }
  statement.append( 1, c );
  while( stream.good() && level )
  {
    c = stream.get();
    if( c == '\\' )
    {
      last_white = false;
      last_escape = !last_escape;
    }
    else if( c == '"' )
    {
      last_white = false;
      if( !last_escape )
      {
        inside_dq = !inside_dq;
      }
    }
    else if( !inside_dq )
    {
      if( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
      {
        if ( !last_white )
        {
          c = ' ';
          last_white = true;
        }
        else
        {
          continue;
        }
      }
      else
      {
        last_white = false;
        if( c == '(' )
        {
          level++;
        }
        else if( c == ')' )
        {
          level--;
        }
      }
    }
    statement.append( 1, c );
  }
  return statement;
}*/

/*std::pair< std::string, std::string > DefsParser::get_enum_values( const std::string& values )
{
  size_t bidx( values.find_first_of( "\"" ) );
  size_t eidx( std::string::npos );
  std::string name;
  std::string value;
  
  if ( bidx == std::string::npos )
  {
    return std::pair< std::string, std::string >();
  }
  while ( true )
  {
    bidx = values.find_first_of( "\"", bidx + 1 );
    if ( bidx == std::string::npos )
    {
      return std::pair< std::string, std::string >();
    }
    else if ( values[ bidx - 1 ] != '\\' )
    {
      break;
    }
  }
  bidx = values.find_first_of( "\"", bidx + 1 );
  if ( bidx == std::string::npos )
  {
    return std::pair< std::string, std::string >();
  }
  eidx = bidx;
  while ( true )
  {
    eidx = values.find_first_of( "\"", eidx + 1 );
    if ( eidx == std::string::npos )
    {
      return std::pair< std::string, std::string >();
    }
    else if ( values[ eidx - 1 ] != '\\' )
    {
      break;
    }
  }
  bidx++;
  name = values.substr( bidx, eidx - bidx );
  bidx = values.find_first_of( "\"", eidx + 1 );
  if ( bidx == std::string::npos )
  {
    return std::pair< std::string, std::string >();
  }
  eidx = bidx;
  while ( true )
  {
    eidx = values.find_first_of( "\"", eidx + 1 );
    if ( eidx == std::string::npos )
    {
      return std::pair< std::string, std::string >();
    }
    else if ( values[ eidx - 1 ] != '\\' )
    {
      break;
    }
  }
  value = values.substr( bidx, eidx - bidx );
  return std::make_pair( name, value );
}*/
