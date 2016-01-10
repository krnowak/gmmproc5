#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH

#include "base-node-data.hh"
#include "types.hh"

#include <mm/xml/base/xml.hh>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Names final : public BaseNodeData
{
public:
  StrSet&&
  steal ();

private:
  virtual void
  process_node_vfunc (Xml::Base::Node const& node,
                      int depth) override;

  virtual void
  postprocess_node_vfunc (Xml::Base::Node const& node,
                          int depth) override;

  StrSet names;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
