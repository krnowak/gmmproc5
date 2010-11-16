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

#ifndef PROC_API_IDENTIFIABLE_H
#define PROC_API_IDENTIFIABLE_H

// standard
#include <string>

/** Main namespace of @b gmmproc5.
 */
namespace Proc
{

/** Namespace containing API definition stuff.
 */
namespace Api
{

/** Base class for object having any sort of id.
 */
class Identifiable
{
public:
  /** Destructor.
   *
   * Not much to say about it.
   */
  virtual ~Identifiable ();

  /** Gets an id of an object.
   *
   * Base classes should reimplement get_id_vfunc().
   *
   * @return An id.
   */
  std::string get_id () const;

  /** Sets an id of an object.
   *
   * Base classes should reimplement set_id_vfunc().
   *
   * @param id An id to set.
   */
  void set_id (const std::string& id);

private:
  /** Pure virtual method getting an id.
   *
   * This method is to be implemented in derived classes.
   *
   * @return An id.
   *
   * @see Identifiable::get_id()
   */
  virtual std::string get_id_vfunc () const = 0;

  /** Pure virtual method setting an id.
   *
   * This method is to be implemented in derived classes.
   *
   * @param id An id to set.
   *
   * @see Identifiable::set_id()
   */
  virtual void set_id_vfunc (const std::string& id) = 0;
};

/** Checks if both @a lhs and @a rhs has equal ids.
 *
 * @param lhs First Identifiable to compare.
 * @param rhs Second Identifiable to compare.
 *
 * @return @c true if ids are equal, otherwise @c false.
 *
 * @relates Identifiable
 */
bool operator== (const Identifiable& lhs, const Identifiable& rhs);

/** Checks if both @a lhs and @a rhs has different ids.
 *
 * @param lhs First Identifiable to compare.
 * @param rhs Second Identifiable to compare.
 *
 * @return @c true if ids are different, otherwise @c false.
 *
 * @relates Identifiable
 */
bool operator!= (const Identifiable& lhs, const Identifiable& rhs);

/** Checks if @a lhs id is lexicographically lower than @a rhs id.
 *
 * @param lhs First Identifiable to compare.
 * @param rhs Second Identifiable to compare.
 *
 * @return @c true if is lower, otherwise @c false.
 *
 * @relates Identifiable
 */
bool operator< (const Identifiable& lhs, const Identifiable& rhs);

/** Checks if @a lhs id is lexicographically lower or equal than @a rhs id.
 *
 * @param lhs First Identifiable to compare.
 * @param rhs Second Identifiable to compare.
 *
 * @return @c true if is lower or equal, otherwise @c false.
 *
 * @relates Identifiable
 */
bool operator<= (const Identifiable& lhs, const Identifiable& rhs);

/** Checks if @a lhs id is lexicographically greater than @a rhs id.
 *
 * @param lhs First Identifiable to compare.
 * @param rhs Second Identifiable to compare.
 *
 * @return @c true if is greater, otherwise @c false.
 *
 * @relates Identifiable
 */
bool operator> (const Identifiable& lhs, const Identifiable& rhs);

/** Checks if @a lhs id is lexicographically greater or equal than @a rhs id.
 *
 * @param lhs First Identifiable to compare.
 * @param rhs Second Identifiable to compare.
 *
 * @return @c true if is greater or equal, otherwise @c false.
 *
 * @relates Identifiable
 */
bool operator>= (const Identifiable& lhs, const Identifiable& rhs);

} // namespace Api

} // namespace Proc

#endif // PROC_API_IDENTIFIABLE_H
