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
#include <algorithm>

// common
#include "apitemplates.h"
#include "stlops.h"

// api
#include "function.h"
#include "object.h"
#include "property.h"
#include "signal.h"

namespace Proc
{

namespace Api
{

Object::Object (const std::string& id)
: Id (id),
  m_parent (),
  m_gtype (),
  m_constructors (),
  m_destructor (0),
  m_methods (),
  m_signals (),
  m_properties (),
  m_vfuncs ()
{}

Object::~Object()
{
  Common::PairDeleter<std::string, Function> pair_deleter_str_fun;
  std::for_each (m_constructors.begin (), m_constructors.end (), pair_deleter_str_fun);
  Common::PtrDeleter<Function> () (m_destructor);
  std::for_each (m_methods.begin (), m_methods.end (), pair_deleter_str_fun);
  std::for_each (m_signals.begin (), m_signals.end (), Common::PairDeleter<std::string, Signal> ());
  std::for_each (m_properties.begin (),m_properties.end (),Common::PairDeleter<std::string, Property> ());
  std::for_each (m_vfuncs.begin (), m_vfuncs.end (), pair_deleter_str_fun);
}

bool Object::set_parent (const std::string& parent)
{
  return Common::FieldAssigner<std::string> () (m_parent, parent);
}

bool Object::set_gtype (const std::string& gtype)
{
  return Common::FieldAssigner<std::string> () (m_gtype, gtype);
}

bool Object::add_constructor (Function* constructor)
{
  return Common::IdInserter<Function> () (m_constructors, constructor);
}

bool Object::set_destructor (Function* destructor)
{
  return Common::FieldAssigner<Function*> () (m_destructor, destructor);
}

bool Object::add_method (Function* method)
{
  return Common::IdInserter<Function> () (m_methods, method);
}

bool Object::add_signal (Signal* signal)
{
  return Common::IdInserter<Signal> () (m_signals, signal);
}

bool Object::add_property (Property* property)
{
  return Common::IdInserter<Property> () (m_properties, property);
}

bool Object::add_vfunc (Function* vfunc)
{
  return Common::IdInserter<Function> () (m_vfuncs, vfunc);
}

} // namespace Api

} // namespace Proc
