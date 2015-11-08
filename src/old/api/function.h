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

#ifndef PROC_API_FUNCTION_H
#define PROC_API_FUNCTION_H

// standard
#include <list>
#include <memory>
#include <string>

// api
#include "functionbase.h"

namespace Proc
{

namespace Api
{

/** Class describing a function.
 */
class Function : public FunctionBase
{
public:
  /** Constructor.
   *
   * Sets empty name, empty parameter list, empty return type and no var args.
   */
  Function ();

  /** Constructor.
   *
   * Sets name as given, empty parameter list, empty return type and no var
   * args.
   *
   * @param id An id to set.
   */
  explicit Function (const std::string& id);

  /** Destructor.
   *
   * Not much to say about it.
   */
  virtual ~Function ();

  /** Gets whether function takes varargs.
   *
   * @return @c true if function takes varargs, otherwise @c false.
   */
  bool get_has_varargs () const;

  /** Sets whether function takes varargs.
   *
   * @param has_varargs An option to use.
   */
  void set_has_varargs (bool has_varargs);

  /** Swaps contents of this function and given one.
   *
   * @param function Another function.
   */
  void swap (Function& function);

private:
  struct FunctionImpl;
  std::shared_ptr<FunctionImpl> m_pimpl;

  virtual std::string get_id_vfunc () const;
  virtual void set_id_vfunc (const std::string& id);

  virtual bool get_wrapped_vfunc () const;
  virtual void set_wrapped_vfunc (bool wrapped);

  virtual std::string get_ret_type_vfunc () const;
  virtual void set_ret_type_vfunc (const std::string& ret_type);

  virtual ParamList::const_iterator get_begin_vfunc () const;
  virtual ParamList::iterator get_begin_vfunc ();
  virtual ParamList::const_iterator get_end_vfunc () const;
  virtual ParamList::iterator get_end_vfunc ();

  virtual void append_param_vfunc (const ParamPtr& param);
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_FUNCTION_H
