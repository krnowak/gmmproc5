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

#ifndef PROC_PARSERS_API_PARSERS_STATEMENT_H
#define PROC_PARSERS_API_PARSERS_STATEMENT_H

// standard
#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

class Statement
{
public:
  typedef std::pair<std::list<std::vector<std::string> >::iterator, bool> IteratorBoolPair;
  typedef std::pair<std::list<std::vector<std::string> >::const_iterator, bool> ConstIteratorBoolPair;
  typedef std::pair<std::string, bool> StringBoolPair;
  typedef std::pair<bool, bool> BoolBoolPair;

                                                        Statement ();
  virtual                                               ~Statement ();

  std::string                                           get_type () const;
  void                                                  set_type (const std::string& type);

  std::string                                           get_header () const;
  void                                                  set_header (const std::string& header);

  StringBoolPair                                        get_value (const std::string& key) const;
  void                                                  set_value (const std::string& key, const std::string& value);

  BoolBoolPair                                          get_bool (const std::string& key) const;
  void                                                  set_bool (const std::string& key, bool value);

  IteratorBoolPair                                      get_list_begin (const std::string& key);
  ConstIteratorBoolPair                                 get_list_begin (const std::string& key) const;
  IteratorBoolPair                                      get_list_end (const std::string& key);
  ConstIteratorBoolPair                                 get_list_end (const std::string& key) const;
  void                                                  set_list (const std::string& key, const std::list<std::vector<std::string> >& list);

  std::string                                           get_file () const;
  void                                                  set_file (const std::string& file);

  unsigned int                                          get_line () const;
  void                                                  set_line (unsigned int line);

  void                                                  clear();
private:
  typedef std::unordered_map<std::string, std::string> StringStringMap;
  typedef std::unordered_map<std::string, bool> StringBoolMap;
  typedef std::unordered_map<std::string, std::list<std::vector<std::string> > > StringListMap;

  std::string                                           m_type;
  std::string                                           m_header;
  StringStringMap                                       m_values;
  StringBoolMap                                         m_bools;
  StringListMap                                         m_lists;
  std::string                                           m_file;
  unsigned int                                          m_line;
};

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc

#endif // PROC_PARSERS_API_PARSERS_STATEMENT_H
