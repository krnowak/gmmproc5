#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH

#include "base.hh"
#include "schema-data.hh"
#include "types.hh"

#include <limits>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Short final : public Base
{
public:
  StrMap<ShortNode>&&
  steal ();

private:
  virtual void
  process_node_vfunc(Xml::Base::Node const& node, int depth) override;

  virtual void
  postprocess_node_vfunc(Xml::Base::Node const& node, int depth) override;

  StrMap<ShortNode> nodes;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_SHORT_HH
