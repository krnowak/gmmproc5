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
Long::process_node (pugi::xml_node const&)
{}

void
Long::process_document (pugi::xml_node const&)
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
