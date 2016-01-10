#include "base.hh"

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

void
Base::process_node (Xml::Base::Node const& node,
                    int depth)
{
  process_node_vfunc (node, depth);
}

void
Base::postprocess_node (Xml::Base::Node const& node,
                        int depth)
{
  postprocess_node_vfunc (node, depth);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
