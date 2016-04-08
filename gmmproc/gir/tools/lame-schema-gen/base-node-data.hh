#ifndef GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_BASE_NODE_DATA_HH
#define GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_BASE_NODE_DATA_HH

#include <gmmproc/xml/xml.hh>

namespace Gmmproc
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class BaseNodeData
{
public:
  void
  process_node (Xml::Node const& node,
                int depth);

  void
  postprocess_node (Xml::Node const& node,
                    int depth);

  void
  wrap_up ();

private:
  virtual void
  process_node_vfunc (Xml::Node const& node,
                      int depth) = 0;

  virtual void
  postprocess_node_vfunc (Xml::Node const& node,
                          int depth) = 0;

  virtual void
  wrap_up_vfunc () = 0;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Gmmproc

#endif // GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_BASE_NODE_DATA_HH
