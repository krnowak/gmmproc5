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
#include <algorithm>
// common
#include "apitemplates.h"
#include "stlops.h"
// api
#include "enum.h"

namespace Proc
{

namespace Api
{

//typedef std::pair<std::string, std::string> Element;

Enum::Enum (const std::string& id)
: Id (id),
  m_elements (),
  m_is_flags (false)
{}
  
Enum::Enum (const Enum& enumeration)
: Id (enumeration),
  m_elements (enumeration.m_elements.size ()),
  m_is_flags (enumeration.m_is_flags)
{
  std::transform (enumeration.m_elements.begin (), enumeration.m_elements.end (), m_elements.begin(), ::Proc::Common::PtrCopier<Element> ());
}

Enum& Enum::operator= (const Enum& enumeration)
{
  if (this != &enumeration)
  {
    *static_cast<Id*> (this) = enumeration;
    std::for_each (m_elements.begin(), m_elements.end(), ::Proc::Common::PtrDeleter<Element> ());
    m_elements.resize (enumeration.m_elements.size ());
    std::transform (enumeration.m_elements.begin (), enumeration.m_elements.end (), m_elements.begin(), ::Proc::Common::PtrCopier<Element> ());
  }
  return *this;
}
  
Enum::~Enum()
{
  std::for_each (m_elements.begin(), m_elements.end(), ::Proc::Common::PtrDeleter<Element> ());
}

bool Enum::append_element (Element* element)
{
  return ::Proc::Common::PlainInserter<Element, std::list<Element*> > () (m_elements, element);
}

} // namespace Api

} // namespace Proc

