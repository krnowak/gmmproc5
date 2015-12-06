#include "base.hh"

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

char const *doc_name = "!@#$_DOCROOT_$#@!";

} // anonymous namespace

void
Base::process_node (pugi::xml_node const& node, int depth)
{
  process_node_vfunc (node.name (), node, depth);
}

void
Base::process_document (pugi::xml_node const& document)
{
  process_node_vfunc (doc_name, document, 0);
}

// static
char const*
Base::get_doc_name ()
{
  return doc_name;
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
