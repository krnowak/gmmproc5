#ifndef GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH
#define GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH

#include <gmmproc/gir/tools/lame-schema-gen/base-node-data.hh>
#include <gmmproc/gir/tools/lame-schema-gen/schema-data.hh>
#include <gmmproc/gir/tools/lame-schema-gen/types.hh>

#include <gmmproc/xml/xml.hh>

namespace Gmmproc
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

} // namespace Gmmproc

#endif // GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH
