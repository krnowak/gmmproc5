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

#include <iosfwd>
#include <string>
#include <utility>

namespace Proc
{

class Api::Namespace;

namespace Parsers
{

namespace ApiParsers
{

class DefsParser : public ApiParser
{
public:
                  DefsParser( const std::string& filename );
  virtual         ~DefsParser();
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
  
  virtual Api::Namespace* parse_vfunc ();
  virtual std::string     get_ext_vfunc ();

  void                    parse_stream( std::istream& stream );
  std::string             get_statement( std::istream& stream );
  void                    parse_statement( const std::string& statement );
  
  void                    on_include( const std::string& statement );
  void                    on_enum( const std::string& statement );
  void                    on_flags( const std::string& statement );
  void                    on_object( const std::string& statement );
  void                    on_function( const std::string& statement );
  void                    on_method( const std::string& statement );
  void                    on_property( const std::string& statement );
  void                    on_signal( const std::string& statement );
  void                    on_vfunc( const std::string& statement );
  void                    on_omit( const std::string& statement );
  
  void                    create_namespace( DefineType, const std::string& statement );
  void                    create_ef( const std::string& statement, bool flags );
  std::string             extract_parentheses( std::istream& stream );
  std::pair< std::string, std::string> get_enum_element( const std::string& values );

  std::string     m_directory;
  std::istream*   m_stream;
  Api::Namespace* m_namespace;
  std::map< std::string, std::pair< DefineType, void (*)( const std::string& ) > > m_token_types_actions;
};

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc

#endif // PROC_PARSERS_API_PARSERS_DEFSPARSER_H
