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

#ifndef PROC_API_SIGNAL_H
#define PROC_API_SIGNAL_H

// api
#include "functionbase.h"

namespace Proc
{

namespace Api
{

enum RunFlags
{
  RUN_INVALID = 0,
  RUN_FIRST = 1 << 0,
  RUN_LAST = 1 << 1,
  RUN_UNKNOWN = 1 << 2
};

inline RunFlags
operator& (RunFlags first, RunFlags second)
{
  return static_cast<RunFlags> (static_cast<unsigned int> (first) & static_cast<unsigned int> (second));
}

inline RunFlags
operator| (RunFlags first, RunFlags second)
{
  return static_cast<RunFlags> (static_cast<unsigned int> (first) | static_cast<unsigned int> (second));
}

inline RunFlags
operator^ (RunFlags first, RunFlags second)
{
  return static_cast<RunFlags> (static_cast<unsigned int> (first) ^ static_cast<unsigned int> (second));
}

inline RunFlags&
operator|= (RunFlags& first, RunFlags second)
{
  return first = static_cast<RunFlags> (static_cast<unsigned int> (first) | static_cast<unsigned int> (second));
}

inline RunFlags&
operator&= (RunFlags& first, RunFlags second)
{
  return first = static_cast<RunFlags> (static_cast<unsigned int> (first) & static_cast<unsigned int> (second));
}

inline RunFlags&
operator^= (RunFlags& first, RunFlags second)
{
  return first = static_cast<RunFlags> (static_cast<unsigned int> (first) ^ static_cast<unsigned int> (second));
}

inline RunFlags
operator~ (RunFlags first)
{
  return static_cast<RunFlags> (~static_cast<unsigned int> (first));
}

class Signal : public FunctionBase
{
public:
  Signal ();
  explicit Signal (const std::string& id);

  virtual ~Signal ();

  RunFlags get_run_flags () const;
  void set_run_flags (RunFlags when);

  void swap (Signal& signal);

private:
  struct SignalImpl;
  std::shared_ptr<SignalImpl> m_pimpl;

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

#endif // PROC_API_SIGNAL_H
