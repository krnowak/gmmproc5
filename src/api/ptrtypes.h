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

#ifndef PROC_API_PTR_TYPES_H
#define PROC_API_PTR_TYPES_H

// standard
#include <memory>

namespace Proc
{

namespace Api
{

class Enum;
class Function;
class Namespace;
class Object;
class Param;
class Property;
class Signal;

typedef std::shared_ptr<Enum> EnumPtr;
typedef std::shared_ptr<Function> FunctionPtr;
typedef std::shared_ptr<Namespace> NamespacePtr;
typedef std::shared_ptr<Object> ObjectPtr;
typedef std::shared_ptr<Param> ParamPtr;
typedef std::shared_ptr<Property> PropertyPtr;
typedef std::shared_ptr<Signal> SignalPtr;

} // namespace Api

} // namespace Proc

#endif // PROC_API_PTR_TYPES_H
