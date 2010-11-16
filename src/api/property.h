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

#ifndef PROC_API_PROPERTY_H
#define PROC_API_PROPERTY_H

// standard
#include <memory>
#include <string>

// api
#include "wrappable.h"

namespace Proc
{

namespace Api
{

enum PropertyFlags
{
  PROPERTY_INVALID = 0,
  PROPERTY_READABLE = 1 << 0,
  PROPERTY_WRITABLE = 1 << 1,
  PROPERTY_CONSTRUCT = 1 << 2,
  PROPERTY_CONSTRUCT_ONLY = 1 << 3
};

inline PropertyFlags
operator& (PropertyFlags first, PropertyFlags second)
{
  return static_cast<PropertyFlags> (static_cast<unsigned int> (first) & static_cast<unsigned int> (second));
}

inline PropertyFlags
operator| (PropertyFlags first, PropertyFlags second)
{
  return static_cast<PropertyFlags> (static_cast<unsigned int> (first) | static_cast<unsigned int> (second));
}

inline PropertyFlags
operator^ (PropertyFlags first, PropertyFlags second)
{
  return static_cast<PropertyFlags> (static_cast<unsigned int> (first) ^ static_cast<unsigned int> (second));
}

inline PropertyFlags&
operator|= (PropertyFlags& first, PropertyFlags second)
{
  return first = static_cast<PropertyFlags> (static_cast<unsigned int> (first) | static_cast<unsigned int> (second));
}

inline PropertyFlags&
operator&= (PropertyFlags& first, PropertyFlags second)
{
  return first = static_cast<PropertyFlags> (static_cast<unsigned int> (first) & static_cast<unsigned int> (second));
}

inline PropertyFlags&
operator^= (PropertyFlags& first, PropertyFlags second)
{
  return first = static_cast<PropertyFlags> (static_cast<unsigned int> (first) ^ static_cast<unsigned int> (second));
}

inline PropertyFlags
operator~ (PropertyFlags first)
{
  return static_cast<PropertyFlags> (~static_cast<unsigned int> (first));
}

class Property : public Wrappable
{
public:
  Property ();
  Property (const std::string& id);

  virtual ~Property ();

  std::string get_type () const;
  void set_type (const std::string& type);

  PropertyFlags get_flags () const;
  void set_flags (PropertyFlags param_flags);

  void swap (Property& property);
private:
  struct PropertyImpl;
  std::shared_ptr<PropertyImpl> m_pimpl;

  virtual std::string get_id_vfunc () const;
  virtual void set_id_vfunc (const std::string& id);

  virtual bool get_wrapped_vfunc () const;
  virtual void set_wrapped_vfunc (bool wrapped);
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_PROPERTY_H
