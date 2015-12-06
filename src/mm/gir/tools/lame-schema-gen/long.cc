#include "long.hh"

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

void
Long::process_node_vfunc (std::string const&, pugi::xml_node const&, int)
{}

NamedSet<Long::Node>&&
Long::steal ()
{
  return std::move (nodes);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
