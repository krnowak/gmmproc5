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

#ifndef PROC_API_PARAM_H
#define PROC_API_PARAM_H

// standard
#include <memory>
#include <string>

// api
#include "identifiable.h"

namespace Proc
{

namespace Api
{

/** Class describing one parameter of a function or signal.
 */
class Param : public Identifiable
{
public:
  /** Constructor.
   *
   * Sets empty id and type.
   */
  Param ();

  /** Constructor.
   *
   * Sets id and empty type.
   *
   * @param id An id to set.
   */
  explicit Param (const std::string& id);

  /** Destructor.
   *
   * Not much to say about it.
   */
  virtual ~Param ();

  /** Gets parameter type.
   *
   * @return A type.
   */
  std::string get_type () const;

  /** Sets parameter type.
   *
   * @param id A type to set.
   */
  void set_type (const std::string& type);

  /** Swaps contents of this parameter and given one.
   *
   * @param param Another parameter.
   */
  void swap (Param& param);

private:
  struct ParamImpl;
  std::shared_ptr<ParamImpl> m_pimpl;

  std::string get_id_vfunc () const;
  void set_id_vfunc (const std::string& id);
};

} // namespace Api

} // namespace Proc

#endif // PROC_API_PARAM_H
