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

#ifndef PROC_COMMON_SIMPLE_DFSM_H
#define PROC_COMMON_SIMPLE_DFSM_H

// standard
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>

// common
#include "dfsmerror.h"
//#include "internalerror.h"

namespace Proc
{

namespace Common
{

template <typename StateType_, typename InputType_, typename InputString_>
class SimpleDFSM
{
public:
  typedef StateType_ StateType;
  typedef InputType_ InputType;
  typedef InputString_ InputString;
  typedef std::function<InputType (const typename InputString::const_iterator&)> TranslatorFunction;
  typedef std::function<void (const StateType&, const InputType&, const typename InputString::const_iterator&)> CallbackFunction;
  class RouteError : public DFSMError
  {
  public:
    typedef std::list<typename SimpleDFSM::InputType> InputsList;
    typedef std::map<StateType, InputsList> DanglingConnections;
    typedef std::shared_ptr<DanglingConnections> DanglingConnectionsPtr;

    RouteError (const DanglingConnectionsPtr& dangling);

    DanglingConnectionsPtr get_dangling_connections() const;

  private:
    struct RouteErrorImpl;
    std::shared_ptr<RouteErrorImpl> m_pimpl;
  };

  SimpleDFSM ();

  bool add_state (const StateType& type);
  bool remove_state (const StateType& type);

  bool set_start_state (const StateType& type);
  bool add_accept_state (const StateType& type);
  bool remove_accept_state (const StateType& type);

  bool add_input (const InputType& input);
  bool remove_input (const InputType& input);

  bool set_input_translator (const TranslatorFunction& function);
  bool set_entry_callback (const CallbackFunction& function);
  bool set_exit_callback (const CallbackFunction& function);

  bool set_default_next_state_for (const StateType& from, const StateType& to);
  bool set_global_default_next_state (const StateType& type);
  bool set_route (const StateType& from, const StateType& to, const InputType& input);

  bool load_string (const InputString& string);
  bool one_step ();
  void process_whole ();

  bool is_current_state_accepting ();

  void reset ();
  void clean ();

  bool is_valid ();

private:
  struct SimpleDFSMImpl;
  std::shared_ptr<SimpleDFSMImpl> m_pimpl;
};

template <typename StateType_, typename InputType_, typename InputString_>
struct SimpleDFSM<StateType_, InputType_, InputString_>::RouteError::RouteErrorImpl
{
  typedef typename SimpleDFSM<StateType_, InputType_, InputString_>::RouteError Owner;

  RouteErrorImpl (const Owner::DanglingConnectionsPtr& dangling);

  typename Owner::DanglingConnectionsPtr m_dangling;
};

template <typename StateType_, typename InputType_, typename InputString_>
SimpleDFSM<StateType_, InputType_, InputString_>::RouteError::RouteErrorImpl::RouteErrorImpl (const SimpleDFSM<StateType_, InputType_, InputString_>::RouteError::DanglingConnectionsPtr& dangling)
: m_dangling (dangling)
{}

template <typename StateType_, typename InputType_, typename InputString_>
SimpleDFSM<StateType_, InputType_, InputString_>::RouteError::RouteError (const SimpleDFSM<StateType_, InputType_, InputString_>::RouteError::DanglingConnectionsPtr& dangling)
: DFSMError (DFSM_NO_ROUTE),
  m_pimpl (new RouteErrorImpl (dangling))
{}

template <typename StateType_, typename InputType_, typename InputString_>
typename SimpleDFSM<StateType_, InputType_, InputString_>::RouteError::DanglingConnectionsPtr SimpleDFSM<StateType_, InputType_, InputString_>::RouteError::get_dangling_connections() const
{
  return m_pimpl->m_dangling;
}

template <typename StateType_, typename InputType_, typename InputString_>
struct SimpleDFSM<StateType_, InputType_, InputString_>::SimpleDFSMImpl
{
  typedef SimpleDFSM<StateType_, InputType_, InputString_> Owner;
  typedef std::map<typename Owner::InputType, typename Owner::StateType> TransitionsMap;
  typedef std::map<typename Owner::StateType, TransitionsMap> StatesMap;
  typedef std::set<typename Owner::StateType> StatesSet;
  typedef std::set<typename Owner::InputType> InputsSet;
  typedef std::map<typename Owner::StateType, typename Owner::StateType> StateToStateMap;

  SimpleDFSMImpl ();

  bool one_step_real ();
  void is_valid_or_throw ();

  StatesMap m_states;
  typename StatesMap::iterator m_start_state;
  typename StatesMap::iterator m_current_state;
  typename StatesMap::iterator m_global_default_state;
  StateToStateMap m_default_next_states;
  StatesSet m_accepting_states;
  InputsSet m_inputs;
  typename Owner::TranslatorFunction m_input_translator;
  typename Owner::CallbackFunction m_entry_callback;
  typename Owner::CallbackFunction m_exit_callback;
  typename Owner::InputString m_input_string;
  typename Owner::InputString::iterator m_current_input;
};

template <typename StateType_, typename InputType_, typename InputString_>
SimpleDFSM<StateType_, InputType_, InputString_>::SimpleDFSMImpl::SimpleDFSMImpl ()
: m_states(),
  m_start_state (m_states.end ()),
  m_current_state (m_states.end ()),
  m_global_default_state (m_states.end ()),
  m_default_next_states (),
  m_accepting_states (),
  m_inputs (),
  m_input_translator (),
  m_entry_callback (),
  m_exit_callback (),
  m_input_string (),
  m_current_input ()
{}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::SimpleDFSMImpl::one_step_real ()
{
  typename Owner::StateType state (m_current_state->first);

  if (m_current_input != m_input_string.end ())
  {
    typename Owner::InputType input (m_input_translator (m_current_input));

    if (m_exit_callback)
    {
      m_exit_callback(m_current_state->first, input, m_current_input);
    }

    TransitionsMap& trans_map (m_current_state->second);
    typename TransitionsMap::iterator trans (trans_map.find (input));

    if (trans != trans_map.end ())
    {
      m_current_state = m_states.find (trans->second);
    }
    else
    {
      typename StateToStateMap::iterator def_pos (m_default_next_states.find (m_current_state->first));

      if (def_pos != m_default_next_states.end ())
      {
        m_current_state = m_states.find (def_pos->second);
      }
      else if (m_global_default_state != m_states.end ())
      {
        m_current_state = m_global_default_state;
      }
      else
      {
        // shouldn't happen here - it should be reported earlier, in is_valid_or_throw.
        //throw InternalError (__FILE__, __PRETTY_FUNCTION__, __LINE__, "No route specified.");
      }
    }
    state = m_current_state->first;
    if (m_entry_callback)
    {
      m_entry_callback(m_current_state->first, input, m_current_input);
    }
    ++m_current_input;
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
void SimpleDFSM<StateType_, InputType_, InputString_>::SimpleDFSMImpl::is_valid_or_throw ()
{
  if (m_states.empty ())
  {
    // no states specified.
    throw (DFSMError (DFSM_NO_STATES));
  }
  if (m_inputs.empty ())
  {
    // no inputs specified.
    throw (DFSMError (DFSM_NO_INPUTS));
  }
  if (m_accepting_states.empty ())
  {
    // no accepting states specified.
    throw (DFSMError (DFSM_NO_ACCEPT_STATES));
  }
  if (m_start_state == m_states.end ())
  {
    // start state not specified.
    throw (DFSMError (DFSM_NO_START_STATE));
  }
  if (!m_input_translator)
  {
    // no translator function specified.
    throw (DFSMError (DFSM_NO_TRANSLATOR));
  }
  if (m_global_default_state == m_states.end ())
  {
    typename StatesMap::iterator states_end (m_states.end ());
    typename StateToStateMap::iterator def_end (m_default_next_states.end ());
    size_t inputs_count (m_inputs.size ());
    typename Owner::RouteError::DanglingConnectionsPtr dangling (new typename RouteError::DanglingConnections);
    typename InputsSet::iterator inputs_begin (m_inputs.begin ());
    typename InputsSet::iterator inputs_end (m_inputs.end ());

    for (typename StatesMap::iterator it (m_states.begin ()); it != states_end; ++it)
    {
      typename Owner::StateType state (it->first);
      TransitionsMap& trans (it->second);

      if (m_default_next_states.find (state) == def_end && trans.size () != inputs_count)
      {
        // there are unspecified routes.
        typename TransitionsMap::iterator trans_end (trans.end ());
        typename Owner::RouteError::InputsList lacking_inputs;

        for (typename InputsSet::iterator in_it (inputs_begin); in_it != inputs_end; ++in_it)
        {
          typename Owner::InputType in (*in_it);

          if (trans.find (in) == trans_end)
          {
            lacking_inputs.push_back (in);
          }
        }
        dangling->insert (typename RouteError::DanglingConnections::value_type (state, lacking_inputs));
      }
    }
    if (!dangling->empty ())
    {
      throw typename Owner::RouteError (dangling);
    }
  }
}

template <typename StateType_, typename InputType_, typename InputString_>
SimpleDFSM<StateType_, InputType_, InputString_>::SimpleDFSM ()
: m_pimpl (new SimpleDFSMImpl)
{}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::add_state (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& type)
{
  if (m_pimpl->m_states.insert (typename SimpleDFSMImpl::StatesMap::value_type (type, typename SimpleDFSMImpl::TransitionsMap ())).second)
  {
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::remove_state (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& type)
{
  typename SimpleDFSMImpl::StatesMap::iterator position (m_pimpl->m_states.find (type));
  typename SimpleDFSMImpl::StatesMap::iterator states_end (m_pimpl->m_states.end ());

  if (position != states_end)
  {
    if (m_pimpl->m_states.size() > 1)
    {
      if (m_pimpl->m_start_state == position)
      {
        m_pimpl->m_start_state = states_end;
      }
      if (m_pimpl->m_global_default_state == position)
      {
        m_pimpl->m_global_default_state = states_end;
      }
      m_pimpl->m_states.erase (position);
      m_pimpl->m_accepting_states.erase (type);

      typename SimpleDFSMImpl::StateToStateMap::iterator def_end (m_pimpl->m_default_next_states.end ());
      typename SimpleDFSMImpl::StateToStateMap::iterator it (m_pimpl->m_default_next_states.begin ());

      while (it != def_end)
      {
        bool mark (false);

        if (it->first == type || it->second == type)
        {
          m_pimpl->m_default_next_states.erase (it++);
        }
        else
        {
          ++it;
        }
      }
    }
    else
    {
      m_pimpl->m_states.clear ();
      m_pimpl->m_start_state = states_end;
      m_pimpl->m_global_default_state = states_end;
      m_pimpl->m_default_next_states.clear ();
      m_pimpl->m_accepting_states.clear ();
    }
    reset();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::set_start_state (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& type)
{
  typename SimpleDFSMImpl::StatesMap::iterator position (m_pimpl->m_states.find (type));

  if (position != m_pimpl->m_states.end () && m_pimpl->m_start_state != position)
  {
    m_pimpl->m_start_state = position;
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::add_accept_state (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& type)
{
  if (m_pimpl->m_states.find (type) != m_pimpl->m_states.end ())
  {
    if (m_pimpl->m_accepting_states.insert (type).second)
    {
      reset ();
      return true;
    }
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::remove_accept_state (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& type)
{
  if (m_pimpl->m_accepting_states.erase (type))
  {
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::add_input (const SimpleDFSM<StateType_, InputType_, InputString_>::InputType& input)
{
  if (m_pimpl->m_inputs.insert (input).second)
  {
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::remove_input (const SimpleDFSM<StateType_, InputType_, InputString_>::InputType& input)
{
  const bool erased (m_pimpl->m_inputs.erase (input));

  if (erased)
  {
    typename SimpleDFSMImpl::StatesMap::iterator states_end (m_pimpl->m_states.end ());

    for (typename SimpleDFSMImpl::StatesMap::iterator it (m_pimpl->m_states.begin ()); it != states_end; ++it)
    {
      it->second.erase (input);
    }
    reset ();
  }
  return erased;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::set_input_translator (const SimpleDFSM<StateType_, InputType_, InputString_>::TranslatorFunction& function)
{
  if (function)
  {
    m_pimpl->m_input_translator = function;
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::set_entry_callback (const SimpleDFSM<StateType_, InputType_, InputString_>::CallbackFunction& function)
{
  if (function)
  {
    m_pimpl->m_entry_callback = function;
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::set_exit_callback (const SimpleDFSM<StateType_, InputType_, InputString_>::CallbackFunction& function)
{
  if (function)
  {
    m_pimpl->m_exit_callback = function;
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::set_default_next_state_for (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& from, const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& to)
{
  typename SimpleDFSMImpl::StatesMap::iterator position (m_pimpl->m_states.find (from));
  typename SimpleDFSMImpl::StatesMap::iterator states_end (m_pimpl->m_states.end ());

  if (position != states_end && m_pimpl->m_states.find (to) != states_end)
  {
    typename SimpleDFSMImpl::StateToStateMap::iterator def_pos (m_pimpl->m_default_next_states.find (position->first));
    bool changed (false);

    if (def_pos != m_pimpl->m_default_next_states.end ())
    {
      if (def_pos->second != to)
      {
        changed = true;
        def_pos->second = to;
      }
    }
    else if (m_pimpl->m_default_next_states.insert (typename SimpleDFSMImpl::StateToStateMap::value_type (from, to)).second)
    {
      changed = true;
    }
    if (changed)
    {
      reset ();
      return true;
    }
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::set_global_default_next_state (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& type)
{
  typename SimpleDFSMImpl::StatesMap::iterator position (m_pimpl->m_states.find (type));

  if (position != m_pimpl->m_states.end () && m_pimpl->m_global_default_state != position)
  {
    m_pimpl->m_global_default_state = position;
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::set_route (const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& from, const SimpleDFSM<StateType_, InputType_, InputString_>::StateType& to, const SimpleDFSM<StateType_, InputType_, InputString_>::InputType& input)
{
  typename SimpleDFSMImpl::StatesMap::iterator position (m_pimpl->m_states.find (from));
  typename SimpleDFSMImpl::StatesMap::iterator states_end (m_pimpl->m_states.end ());

  if (position != states_end && m_pimpl->m_states.find (to) != states_end)
  {
    typename SimpleDFSMImpl::TransitionsMap::iterator trans (position->second.find (input));

    if (trans != position->second.end ())
    {
      if (trans->second != to)
      {
        trans->second = to;
        reset ();
        return true;
      }
    }
    else if (position->second.insert (typename SimpleDFSMImpl::TransitionsMap::value_type (input, to)).second)
    {
      reset ();
      return true;
    }
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::load_string (const SimpleDFSM<StateType_, InputType_, InputString_>::InputString& string)
{
  if (m_pimpl->m_input_string != string)
  {
    m_pimpl->m_input_string = string;
    reset ();
    return true;
  }
  return false;
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::one_step ()
{
  m_pimpl->is_valid_or_throw();
  return m_pimpl->one_step_real ();
}

template <typename StateType_, typename InputType_, typename InputString_>
void SimpleDFSM<StateType_, InputType_, InputString_>::process_whole ()
{
  m_pimpl->is_valid_or_throw();
  while (m_pimpl->one_step_real ());
}

template <typename StateType_, typename InputType_, typename InputString_>
bool SimpleDFSM<StateType_, InputType_, InputString_>::is_current_state_accepting ()
{
  return m_pimpl->m_accepting_states.find (m_pimpl->m_current_state->first) != m_pimpl->m_accepting_states.end ();
}

template <typename StateType_, typename InputType_, typename InputString_>
void SimpleDFSM<StateType_, InputType_, InputString_>::reset ()
{
  m_pimpl->m_current_state = m_pimpl->m_start_state;
  m_pimpl->m_current_input = m_pimpl->m_input_string.begin ();
}

template <typename StateType_, typename InputType_, typename InputString_>
void SimpleDFSM<StateType_, InputType_, InputString_>::clean ()
{
  typename SimpleDFSMImpl::StatesMap::iterator states_end (m_pimpl->m_states.end ());

  m_pimpl->m_states.clear ();
  m_pimpl->m_default_next_states.clear();
  m_pimpl->m_global_default_state = states_end;
  m_pimpl->m_start_state = states_end;
  m_pimpl->m_accepting_states.clear ();
  m_pimpl->m_inputs.clear ();
  {
    TranslatorFunction empty;
    m_pimpl->m_input_translator.swap (empty);
  }
  {
    CallbackFunction empty;
    m_pimpl->m_entry_callback.swap (empty);
  }
  {
    CallbackFunction empty;
    m_pimpl->m_exit_callback.swap (empty);
  }
  m_pimpl->m_input_string.clear ();
  reset ();
}

} // namespace Common

} // namespace Proc

#endif // PROC_COMMON_SIMPLE_DFSM_H
