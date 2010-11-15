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

#ifndef PROC_API_FUNCTION_BASE_H
#define PROC_API_FUNCTION_BASE_H

// standard
#include <list>
#include <memory>
#include <string>

// api
#include "wrappable.h"

namespace Proc
{

namespace Api
{

class Param;

/** Base class describing function-like stuff.
 */
class FunctionBase : public Wrappable
{
public:
  typedef std::shared_ptr<Param> ParamPtr;
  typedef std::list<ParamPtr> ParamList;

  /** Destructor.
   *
   * Not much to say about it.
   */
  virtual ~FunctionBase ();

  /** Gets return type of a function.
   *
   * Base classes should reimplement get_ret_type_vfunc().
   *
   * @return A return type.
   */
  std::string get_ret_type () const;

  /** Sets return type of a function.
   *
   * Base classes should reimplement set_ret_type_vfunc().
   *
   * @param ret_type A return type to set.
   */
  void set_ret_type (const std::string& ret_type);

  /** Gets iterator to first parameter of this function.
   *
   * Base classes should reimplement get_begin_vfunc().
   *
   * @return A const iterator to the beginning.
   */
  ParamList::const_iterator get_begin () const;

  /** Gets iterator to first parameter of this function.
   *
   * Base classes should reimplement get_begin_vfunc().
   *
   * @return An iterator to the beginning.
   */
  ParamList::iterator get_begin ();

  /** Gets iterator to past last parameter of this function.
   *
   * Base classes should reimplement get_end_vfunc().
   *
   * @return A const iterator to the end.
   */
  ParamList::const_iterator get_end () const;

  /** Gets iterator to past last parameter of this function.
   *
   * Base classes should reimplement get_end_vfunc().
   *
   * @return A iterator to the end.
   */
  ParamList::iterator get_end ();

  /** Appends parameter to the parameter list.
   *
   * Note that no checking of a parameter validity is done.
   *
   * Base classes should reimplement append_param_vfunc().
   *
   * @param param A parameter to be appended.
   */
  void append_param (const ParamPtr& param);

private:
  virtual std::string get_ret_type_vfunc () const = 0;
  virtual void set_ret_type_vfunc (const std::string& ret_type) = 0;

  virtual ParamList::const_iterator get_begin_vfunc () const = 0;
  virtual ParamList::iterator get_begin_vfunc () = 0;
  virtual ParamList::const_iterator get_end_vfunc () const = 0;
  virtual ParamList::iterator get_end_vfunc () = 0;

  virtual void append_param_vfunc (const ParamPtr& param) = 0;
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_FUNCTION_BASE_H
