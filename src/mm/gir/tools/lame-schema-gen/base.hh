#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_BASE_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_BASE_HH

#include "types.hh"

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Base
{
public:
  void
  process_node (pugi::xml_node const& node, int depth);

  void
  process_document (pugi::xml_node const& document);

  static
  char const*
  get_doc_name ();

private:
  void
  virtual process_node_vfunc(std::string const& name, pugi::xml_node const& node, int depth) = 0;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_BASE_HH
