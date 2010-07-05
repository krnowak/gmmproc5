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

#ifndef PROC_PARSERS_API_PARSERS_DEFSPARSER_H
#define PROC_PARSERS_API_PARSERS_DEFSPARSER_H

// standard
#include <iosfwd>
#include <map>
#include <string>
#include <utility>

// parsers
#include "apiparser.h"

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

namespace DefsPrivate
{

class Statement;
class ParserStuff;

} // namespace DefsPrivate

class DefsParser : public ApiParser
{
public:
                            DefsParser (const std::string& filename);
  virtual                   ~DefsParser();
private:
  enum DefineType
  {
    INCLUDE,
    ENUM,
    FLAGS,
    OBJECT,
    FUNCTION,
    METHOD,
    PROPERTY,
    SIGNAL,
    VFUNC,
    OMIT
  };
  
  enum Context
  {
    CONTEXT_OUTSIDE,
    CONTEXT_HEADER,
    CONTEXT_LEVEL1,
    CONTEXT_LEVEL2,
    CONTEXT_LIST_ELEMENT
  };
  
  virtual Api::Namespace*   parse_vfunc ();
  virtual std::string       get_ext_vfunc () const;

  //void                    parse_stream( std::istream& stream );
  //std::string             get_statement( std::istream& stream );
  //void                    parse_statement( const DefsPrivate::Statement& statement );
  
  /*void                    on_include( const DefsPrivate::Statement& statement );
  void                    on_enum( const DefsPrivate::Statement& statement );
  void                    on_flags( const DefsPrivate::Statement& statement );
  void                    on_object( const DefsPrivate::Statement& statement );
  void                    on_function( const DefsPrivate::Statement& statement );
  void                    on_method( const DefsPrivate::Statement& statement );
  void                    on_property( const DefsPrivate::Statement& statement );
  void                    on_signal( const DefsPrivate::Statement& statement );
  void                    on_vfunc( const DefsPrivate::Statement& statement );
  void                    on_omit( const DefsPrivate::Statement& statement );*/
  
  //void                    create_namespace( DefineType type, const DefsPrivate::Statement& statement );
  //void                    create_ef( const std::string& statement, bool flags );
  //std::string             extract_parentheses( std::istream& stream );
  //std::pair< std::string, std::string> get_enum_element( const std::string& values );

  DefsPrivate::ParserStuff* m_stuff;
};

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc

#endif // PROC_PARSERS_API_PARSERS_DEFSPARSER_H
