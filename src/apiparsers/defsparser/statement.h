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

#ifndef PROC_STATEMENT_H
#define PROC_STATEMENT_H

namespace Proc
{

namespace Parsers
{

namespace ApiParsers
{

namespace DefsPrivate
{
/*
class Statement
{
public:
  Statement();
  Statement( const std::string& body );
  virtual ~Statement();
  std::string extract_header() const;
  std::string extract_value (const std::string& key);
  bool extract_bool (const std::string& key);
  std::list< std::vector< std::string> > extract_value_list (const std::string& key);
private:
  size_t get_key_value_index (const std::string& key);
  
  std::string m_body;
};
*/
class Statement
{
public:
  Statement ();
  virtual ~Statement ();
  
  inline const std::string& get_type () const;
  inline std::string get_type ();
  //void set_type (const std::string& type);
  
  inline const std::string& get_header() const;
  inline std::string& get_header();
  //void set_header (const std::string& header);
  
  inline const std::map<std::string, std::string>& get_values() const;
  inline std::map<std::string, std::string>& get_values();
  
  inline const std::map<std::string, bool>& get_bools() const;
  inline std::map<std::string, bool>& get_bools();
  
  inline const std::map<std::string, std::list<std::vector<std::string> > >& get_lists() const;
  inline std::map<std::string, std::list<std::vector<std::string> > >& get_lists();
  
  void clear();
  /*
  bool add_value (const std::string& key, const std::string& value);
  std::map<std::string, std::string>::const_iterator get_value_begin() const;
  std::map<std::string, std::string>::iterator get_value_begin();
  std::map<std::string, std::string>::const_iterator get_value_end() const;
  std::map<std::string, std::string>::iterator get_value_end();
  
  bool add_bool (const std::string& key, bool value);
  std::map<std::string, bool>::const_iterator get_bool_begin () const;
  std::map<std::string, bool>::iterator get_bool_begin ();
  std::map<std::string, bool>::const_iterator get_bool_end () const;
  std::map<std::string, bool>::iterator get_bool_end ();
  
  bool create_list (const std::string& list);
  bool add_to_list (const std::string& list, const std::vector<std::string>& values);
  */
  
private:
  std::string m_type;
  std::string m_header;
  std::map<std::string, std::string> m_values;
  std::map<std::string, bool> m_bools;
  std::map<std::string, std::list<std::vector<std::string> > > m_lists;
};

inline const std::string& Statement::get_type () const
{
  return const_cast<Statement*> (this)->get_type();
}

inline std::string Statement::get_type ()
{
  return m_type;
}

inline const std::string& Statement::get_header() const
{
  return const_cast<Statement*> (this)->get_header();
}

inline std::string& Statement::get_header()
{
  return m_header;
}

inline const std::map<std::string, std::string>& Statement::get_values() const
{
  return const_cast<Statement*> (this)->get_values();
}

inline std::map<std::string, std::string>& Statement::get_values()
{
  return m_values;
}

inline const std::map<std::string, bool>& Statement::get_bools() const
{
  return const_cast<Statement*> (this)->get_bools();
}

inline std::map<std::string, bool>& Statement::get_bools()
{
  return m_bools;
}

inline const std::map<std::string, std::list<std::vector<std::string> > >& Statement::get_lists() const
{
  return const_cast<Statement*> (this)->get_lists();
}

inline std::map<std::string, std::list<std::vector<std::string> > >& Statement::get_lists()
{
  return m_lists;
}

} // namespace DefsPrivate

} // namespace ApiParsers

} // namespace Parsers

} // namespace Proc

#endif // PROC_STATEMENT_H
