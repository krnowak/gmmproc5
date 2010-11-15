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

#ifndef PROC_API_WRAPPABLE
#define PROC_API_WRAPPABLE

// api
#include "identifiable.h"

namespace Proc
{

namespace Api
{

/** Base class for object that can be wrapped.
 */
class Wrappable : public Identifiable
{
public:
  /** Destructor.
   *
   * Not much to say about it.
   */
  ~Wrappable ();

  /** Gets whether this object has been already wrapped.
   *
   * Base classes should reimplement get_wrapped_vfunc().
   *
   * @return @c if this object was already wrapped, otherwise @c false.
   */
  bool get_wrapped () const;

  /** Sets whether this object has been already wrapped.
   *
   * Base classes should reimplement set_wrapped_vfunc().
   *
   * @param wrapped An option to use.
   */
  void set_wrapped (bool wrapped);

private:
  /** Pure virtual method getting wrapping status.
   *
   * This method is to be implemented in derived classes.
   *
   * @return Wrapping status.
   *
   * @see Wrappable::get_wrapped()
   */
  virtual bool get_wrapped_vfunc () const = 0;

  /** Pure virtual method setting wrapping status.
   *
   * This method is to be implemented in derived classes.
   *
   * @param id An option to use.
   *
   * @see Wrappable::set_wrapped()
   */
  virtual void set_wrapped_vfunc (bool wrapped) = 0;
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_WRAPPABLE
