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

#include "dfsmerror.h"

namespace Proc
{

namespace Common
{

struct DFSMError::DFSMErrorImpl
{
  DFSMErrorImpl (DFSMErrorCode error_code);

  static const char* create_message (DFSMErrorCode error_code);

  DFSMErrorCode m_code;
};

DFSMError::DFSMErrorImpl::DFSMErrorImpl (DFSMErrorCode error_code)
: m_code (error_code)
{}

const char* DFSMError::DFSMErrorImpl::create_message (DFSMErrorCode error_code)
{
  switch (error_code)
  {
    case DFSM_NO_STATES:
    {
      return "No states specified.";
    }
    case DFSM_NO_START_STATE:
    {
      return "No start state specified.";
    }
    case DFSM_NO_ACCEPT_STATES:
    {
      return "No accept states specified.";
    }
    case DFSM_NO_INPUTS:
    {
      return "No inputs specified.";
    }
    case DFSM_NO_TRANSLATOR:
    {
      return "No translator specified.";
    }
    case DFSM_BUGGY_TRANSLATOR:
    {
      return "Translator returned wrong input.";
    }
    case DFSM_NO_ROUTE:
    {
      return "No route specified.";
    }
    default:
    {
      return "Unknown error.";
    }
  }
}

DFSMError::DFSMError (DFSMErrorCode error_code)
: std::logic_error (DFSMErrorImpl::create_message (error_code)),
  m_pimpl (new DFSMErrorImpl (error_code))
{}

DFSMErrorCode DFSMError::get_code () const
{
  return m_pimpl->m_code;
}

} // namespace Common

} // namespace Proc
