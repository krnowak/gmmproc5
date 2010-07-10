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

#ifndef PROC_COMMON_INTERNAL_ERROR_H
#define PROC_COMMON_INTERNAL_ERROR_H

// standard
#include <stdexcept>

namespace Proc
{

namespace Common
{

class InternalError : public std::runtime_error
{
public:
  explicit            InternalError (const char* file, const char* function, unsigned int line, const std::string& what_arg) throw ();
  virtual             ~InternalError () throw ();
private:
  static std::string  create_string (const char* file, const char* function, unsigned int line, const std::string& what_arg) throw ();
};

} // namespace Common

} // namespace Proc

#endif // PROC_COMMON_INTERNAL_ERROR_H
