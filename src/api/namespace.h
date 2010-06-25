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

#ifndef PROC_API_NAMESPACE_H
#define PROC_API_NAMESPACE_H

// standard
#include <map>
#include <string>
// api
#include "id.h"

namespace Proc
{

namespace Api
{

class Enum;
class Object;
class Function;

class Namespace : public Id
{
public:
  Namespace (const std::string& id = std::string ());
  
  Namespace (const Namespace& ns);
  Namespace& operator= (const Namespace& ns);
  
  virtual ~Namespace ();
  
  bool add_enum (Enum* enumeration);
  
  bool add_object (Object* object);
  
  bool add_function (Function* function);
  
  inline bool operator== (const Namespace& ns);
  
  static std::string get_namespace_name( const std::string& name );
private:
  std::map< std::string, Enum* > m_enums;
  std::map< std::string, Object* > m_objects;
  std::map< std::string, Function* > m_functions;
};

inline bool Namespace::operator== (const Namespace& ns)
{
  const Id* id (this);
  return (*id == ns);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_NAMESPACE_H
