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

//common
#include "apitemplates.h"
//api
#include "signal.h"

namespace Proc
{

namespace Common
{

template <>
struct ZeroTraits<Proc::Api::RunFlags>
{
  typedef Proc::Api::RunFlags Type;
  static const Type zero;
};

const ZeroTraits<Proc::Api::RunFlags>::Type ZeroTraits<Proc::Api::RunFlags>::zero = Proc::Api::RUN_INVALID;

} // namespace Common

namespace Api
{

Signal::Signal (const std::string& id)
: Function (id),
  m_run_flags (RUN_INVALID)
{}

Signal::~Signal ()
{}

bool Signal::set_run_flags (RunFlags run_flags)
{
  return ::Proc::Common::FieldAssigner<RunFlags> () (m_run_flags, run_flags);
}

} // namespace Api

} // namespace Proc
