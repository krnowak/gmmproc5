#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH

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
try_emplace(std::unordered_set<std::string>& names, pugi::char_t const* name)
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

  for (auto attr = node.attributes ())
  {
    try_emplace (names, attr.name ());
  }
}

std::unordered_set<std::string>&&
Names::steal ()
{
  return std::move (names);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
