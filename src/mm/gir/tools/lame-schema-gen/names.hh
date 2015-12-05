#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH

#include "types.hh"

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Names
{
public:
  Names() = default;

  void
  process_node (pugi::xml_node const& node);

  void
  process_document (pugi::xml_node const&)
  {}

  StrSet&&
  steal ();

private:
  StrSet names;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
