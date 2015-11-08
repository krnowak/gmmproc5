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

#ifndef PROC_API_OBJECT_H
#define PROC_API_OBJECT_H

// standard
#include <memory>
#include <string>

// api
#include "ptrtypes.h"
#include "wrappable.h"

namespace Proc
{

namespace Api
{

class Function;
class Property;
class Signal;

class Object : public Wrappable
{
public:

  Object ();
  Object (const std::string& id);

  virtual ~Object ();

  std::string get_parent () const;
  void set_parent ( const std::string& parent );

  std::string get_gtype () const;
  void set_gtype ( const std::string& gtype );

  bool add_constructor (const FunctionPtr& constructor);
  FunctionPtr get_constructor (const std::string& name) const;
  void set_destructor (const FunctionPtr& destructor);
  FunctionPtr get_destructor () const;
  bool add_method (const FunctionPtr& method);
  FunctionPtr get_method (const std::string& name) const;
  bool add_signal (const SignalPtr& signal);
  SignalPtr get_signal (const std::string& name) const;
  bool add_property (const PropertyPtr& property);
  PropertyPtr get_property (const std::string& name) const;
  bool add_vfunc (const FunctionPtr& vfunc);
  FunctionPtr get_vfunc (const std::string& name) const;

  bool get_is_interface () const;
  void set_is_interface (bool iface);

  void swap (Object& object);

private:
  struct ObjectImpl;
  std::shared_ptr<ObjectImpl> m_pimpl;

  virtual std::string get_id_vfunc () const;
  virtual void set_id_vfunc (const std::string& id);

  virtual bool get_wrapped_vfunc () const;
  virtual void set_wrapped_vfunc (bool wrapped);
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_OBJECT_H
