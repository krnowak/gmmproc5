#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH

#include "base-node-data.hh"
#include "schema-data.hh"
#include "types.hh"

#include <mm/xml/xml.hh>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Short final : public BaseNodeData
{
public:
  StrMap<ShortNode>&&
  steal ();

private:
  virtual void
  process_node_vfunc (Xml::Node const& node, int depth) override;

  virtual void
  postprocess_node_vfunc (Xml::Node const& node, int depth) override;

  virtual void
  wrap_up_vfunc () override;

  StrMap<ShortNode> nodes;
  Str toplevel_name;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH
