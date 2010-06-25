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

// common
#include "apitemplates.h"
// api
#include "property.h"

namespace Proc
{

namespace Common
{

template <>
struct ZeroTraits<Proc::Api::ParamFlags>
{
  typedef Proc::Api::ParamFlags Type;
  static const Type zero;
};

const ZeroTraits<Proc::Api::ParamFlags>::Type ZeroTraits<Proc::Api::ParamFlags>::zero = ::Proc::Api::PARAM_INVALID;

} // namespace Common

namespace Api
{

Property::Property(const std::string& id)
: Id (id),
  m_type (),
  m_param_flags (PARAM_INVALID)
{}

Property::~Property()
{}

bool Property::set_type(const std::string& type)
{
  return ::Proc::Common::FieldAssigner<std::string> () (m_type, type);
}

bool Property::set_flags(ParamFlags param_flags)
{
  return ::Proc::Common::FieldAssigner<ParamFlags> () (m_param_flags, param_flags);
}

} // namespace Api

} // namespace Proc
