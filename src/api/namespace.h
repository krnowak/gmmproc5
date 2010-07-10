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
#include <string>
#include <unordered_map>

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
  Namespace&          operator= (const Namespace& ns);

  virtual             ~Namespace ();

  bool                add_enum (Enum* enumeration);
  Enum*               get_enum (const std::string& c_name) const;

  bool                add_object (Object* object);
  Object*             get_object (const std::string& c_name) const;

  bool                add_function (Function* function);
  Function*           get_function (const std::string& c_name) const;

  inline bool         operator== (const Namespace& ns);

  static std::string  get_namespace_name (const std::string& name);
private:
  typedef std::unordered_map<std::string, Enum*> StringEnumMap;
  typedef std::unordered_map<std::string, Object*> StringObjectMap;
  typedef std::unordered_map<std::string, Function*> StringFunctionMap;

  static bool         underline_break (const std::string::const_iterator& it);
  static bool         capital_break (const std::string::const_iterator& it);

  StringEnumMap       m_enums;
  StringObjectMap     m_objects;
  StringFunctionMap   m_functions;
};

inline bool Namespace::operator== (const Namespace& ns)
{
  const Id* id (this);
  return (*id == ns);
}

} // namespace Api

} // namespace Proc

#endif // PROC_API_NAMESPACE_H
