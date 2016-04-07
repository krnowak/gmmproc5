#include "base-node-data.hh"

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

void
BaseNodeData::process_node (Xml::Node const& node,
                            int depth)
{
  process_node_vfunc (node, depth);
}

void
BaseNodeData::postprocess_node (Xml::Node const& node,
                                int depth)
{
  postprocess_node_vfunc (node, depth);
}

void
BaseNodeData::wrap_up ()
{
  wrap_up_vfunc ();
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
