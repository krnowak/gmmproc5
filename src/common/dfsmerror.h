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

#ifndef PROC_COMMON_DFSM_ERROR_H
#define PROC_COMMON_DFSM_ERROR_H

#include <memory>
#include <stdexcept>

namespace Proc
{

namespace Common
{

enum DFSMErrorCode
{
  DFSM_NO_STATES,
  DFSM_NO_START_STATE,
  DFSM_NO_ACCEPT_STATES,
  DFSM_NO_INPUTS,
  DFSM_NO_TRANSLATOR,
  DFSM_BUGGY_TRANSLATOR,
  DFSM_NO_ROUTE
};

class DFSMError : public std::logic_error
{
public:
  DFSMError (DFSMErrorCode error_code);

  DFSMErrorCode get_code () const;

private:
  struct DFSMErrorImpl;
  std::shared_ptr<DFSMErrorImpl> m_pimpl;
};

} // namespace Common

} // namespace Proc

#endif // PROC_COMMON_DFSM_ERROR_H
