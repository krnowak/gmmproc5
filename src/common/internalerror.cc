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
#include <sstream>

// common
#include "internalerror.h"

namespace Proc
{

namespace Common
{

InternalError::InternalError (const char* file, const char* function, unsigned int line, const std::string& what_arg) throw ()
: std::runtime_error (create_string (file, function, line, what_arg))
{}

InternalError::~InternalError () throw ()
{}

std::string InternalError::create_string (const char* file, const char* function, unsigned int line, const std::string& what_arg) throw ()
{
  std::ostringstream oss;

  oss << "Internal error in " << file << ", " << function << ", " << line << ": " << what_arg;
  return oss.str ();
}

} // namespace Common

} // namespace Proc
