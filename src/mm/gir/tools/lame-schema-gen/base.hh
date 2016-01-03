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
  process_node (Xml::Base::Node const& node, int depth);

  void
  postprocess_node (Xml::Base::Node const& node, int depth);

private:
  virtual void
  process_node_vfunc (Xml::Base::Node const& node, int depth) = 0;

  virtual void
  postprocess_node_vfunc (Xml::Base::Node const& node, int depth) = 0;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_BASE_HH
