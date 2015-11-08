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

#ifndef PROC_API_ENUM_H
#define PROC_API_ENUM_H

// standard
#include <list>
#include <memory>
#include <string>
#include <utility>

// api
#include "wrappable.h"

namespace Proc
{

namespace Api
{

/* Class describing enumeration/flags.
 */
class Enum : public Wrappable
{
public:
  typedef std::pair<std::string, std::string> Element;
  typedef std::shared_ptr<Element> ElementPtr;
  typedef std::list<ElementPtr> ElementList;

  /** Constructor.
   *
   * Sets empty id and empty element list. Marks it as yet unwrapped.
   */
  Enum ();

  /** Constructor.
   *
   * Sets id as given and empty element list. Marks it as yet unwrapped.
   *
   * @param id An id to set.
   */
  explicit Enum (const std::string& id);

  /** Destructor.
   *
   * Not much to say about it.
   */
  virtual ~Enum();

  /** Gets whether this enumeration is marked as flags.
   *
   * Flags means that for this enumeration also some bitwise operators should be
   * set.
   *
   * @return @c true if this enumeration is treated as flags, otherwise
   * @c false.
   */
  bool get_is_flags () const;

  /** Sets whether this enumeration is marked as flags.
   *
   * Flags means that for this enumeration also some bitwise operators should be
   * set.
   *
   * @param is_flags An option to use.
   */
  void set_is_flags (bool is_flags);

  /** Gets iterator to first element of this enum.
   *
   * @return A const iterator to the beginning.
   */
  ElementList::const_iterator get_begin () const;

  /** Gets iterator to first element of this enum.
   *
   * @return An iterator to the beginning.
   */
  ElementList::iterator get_begin ();

  /** Gets iterator to past last element of this enum.
   *
   * @return A const iterator to the end.
   */
  ElementList::const_iterator get_end () const;

  /** Gets iterator to past last element of this enum.
   *
   * @return An iterator to the end.
   */
  ElementList::iterator get_end ();

  /** Appends element to the element list.
   *
   * Note that no checking of an element validity is done.
   *
   * @param element An element to be appended.
   */
  void append_element (const ElementPtr& element);

  /** Swaps contents of this enum and given one.
   *
   * @param enumeration Another enum.
   */
  void swap (Enum& enumeration);

private:
  struct EnumImpl;
  std::shared_ptr<EnumImpl> m_pimpl;

  virtual std::string get_id_vfunc () const;
  virtual void set_id_vfunc (const std::string& id);

  virtual bool get_wrapped_vfunc () const;
  virtual void set_wrapped_vfunc (bool wrapped);
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_ENUM_H
