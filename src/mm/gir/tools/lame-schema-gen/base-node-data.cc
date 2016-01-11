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
BaseNodeData::process_node (Xml::Base::Node const& node,
                            int depth)
{
  process_node_vfunc (node, depth);
}

void
BaseNodeData::postprocess_node (Xml::Base::Node const& node,
                                int depth)
{
  postprocess_node_vfunc (node, depth);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
