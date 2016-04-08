#ifndef GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
#define GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH

#include <gmmproc/gir/tools/lame-schema-gen/base-node-data.hh>
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

class Names final : public BaseNodeData
{
public:
  StrSet&&
  steal ();

private:
  virtual void
  process_node_vfunc (Xml::Node const& node,
                      int depth) override;

  virtual void
  postprocess_node_vfunc (Xml::Node const& node,
                          int depth) override;

  virtual void
  wrap_up_vfunc () override;

  StrSet names;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Gmmproc

#endif // GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_NAMES_HH
