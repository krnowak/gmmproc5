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
#include <memory>
#include <string>

// api
#include "identifiable.h"
#include "ptrtypes.h"

namespace Proc
{

namespace Api
{

class Enum;
class Object;
class Function;

class Namespace : public Identifiable
{
public:
  Namespace ();
  Namespace (const std::string& id);

  virtual ~Namespace ();

  bool add_enum (const EnumPtr& enumeration);
  EnumPtr get_enum (const std::string& c_name) const;

  bool add_object (const ObjectPtr& object);
  ObjectPtr get_object (const std::string& c_name) const;

  bool add_function (const FunctionPtr& function);
  FunctionPtr get_function (const std::string& c_name) const;

  static std::string get_namespace_name (const std::string& name);

  void swap (Namespace& ns);

private:
  struct NamespaceImpl;
  std::shared_ptr<NamespaceImpl> m_pimpl;

  virtual std::string get_id_vfunc () const;
  virtual void set_id_vfunc (const std::string& id);
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_NAMESPACE_H
