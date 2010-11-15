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

// api
#include "signal.h"

namespace Proc
{

namespace Api
{

struct Signal::SignalImpl
{
  SignalImpl ();
  SignalImpl (const std::string& id);

  std::string m_id;
  bool m_wrapped;
  std::string m_ret_type;
  Signal::ParamList m_parameters;
  RunFlags m_run_flags;
};

Signal::SignalImpl::SignalImpl ()
: m_id (),
  m_wrapped (false),
  m_ret_type (),
  m_parameters (),
  m_run_flags (RUN_INVALID)
{}

Signal::SignalImpl::SignalImpl (const std::string& id)
: m_id (id),
  m_wrapped (false),
  m_ret_type (),
  m_parameters (),
  m_run_flags (RUN_INVALID)
{}

Signal::Signal ()
: FunctionBase (),
  m_pimpl (new SignalImpl)
{}

Signal::Signal (const std::string& id)
: FunctionBase (),
  m_pimpl (new SignalImpl (id))
{}

Signal::~Signal ()
{}

RunFlags Signal::get_run_flags () const
{
  return m_pimpl->m_run_flags;
}

void Signal::set_run_flags (RunFlags run_flags)
{
  m_pimpl->m_run_flags = run_flags;
}

void Signal::swap (Signal& signal)
{
  m_pimpl.swap (signal.m_pimpl);
}

void Signal::set_ret_type_vfunc (const std::string& ret_type)
{
  m_pimpl->m_ret_type = ret_type;
}

std::string Signal::get_ret_type_vfunc () const
{
  return m_pimpl->m_ret_type;
}

Signal::ParamList::const_iterator Signal::get_begin_vfunc () const
{
  return m_pimpl->m_parameters.begin ();
}

Signal::ParamList::iterator Signal::get_begin_vfunc ()
{
  return m_pimpl->m_parameters.begin ();
}

Signal::ParamList::const_iterator Signal::get_end_vfunc () const
{
  return m_pimpl->m_parameters.end ();
}

Signal::ParamList::iterator Signal::get_end_vfunc ()
{
  return m_pimpl->m_parameters.end ();
}

void Signal::append_param_vfunc (const ParamPtr& param)
{
  m_pimpl->m_parameters.push_back (param);
}

} // namespace Api

} // namespace Proc
