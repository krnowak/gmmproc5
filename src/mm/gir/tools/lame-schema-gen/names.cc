#include "names.hh"

#include <pugixml.hpp>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

namespace
{

void
try_emplace (StrSet& names, pugi::char_t const* name)
{
  if (name && *name)
  {
    names.emplace (name);
  }
}

} // anonymous namespace

void
Names::process_node (pugi::xml_node const& node)
{
  try_emplace (names, node.name ());

  for (auto attr : node.attributes ())
  {
    try_emplace (names, attr.name ());
  }
}

StrSet&&
Names::steal ()
{
  return std::move (names);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
