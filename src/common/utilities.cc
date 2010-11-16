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
#include <fstream>
#include <sstream>

// common
#include "utilities.h"

namespace Proc
{

namespace Common
{

namespace Utilities
{

std::string read_contents (const std::string& path)
{
  std::ifstream file (path.c_str ());
  std::ostringstream oss;

  if (file.good ())
  {
    file >> oss.rdbuf ();
  }
  return oss.str ();
}

std::string dir_name (const std::string& path)
{
  const size_t found (path.find_last_of ("/"));

  if (found == std::string::npos)
  {
    return std::string ("./");
  }
  return path.substr (0, found + 1);
}

} // namespace Utilities

} // namespace Common

} // namespace Proc
