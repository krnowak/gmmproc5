#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH

#include "base-node-data.hh"
#include "schema-data.hh"
#include "types.hh"

#include <mm/xml/base/xml.hh>

#include <functional>
#include <memory>
#include <stack>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

class Long final : public BaseNodeData
{
public:
  std::unique_ptr<LongNode>&&
  steal ();

private:
  virtual void
  process_node_vfunc (Xml::Base::Node const& node,
                      int depth) override;

  virtual void
  postprocess_node_vfunc (Xml::Base::Node const& node,
                          int depth) override;

  void
  process_toplevel (Xml::Base::Node const& node,
                    int depth);

  void
  process_element (Xml::Base::Node const& node,
                   int depth);

  void
  setup_node_stack_for_depth (int depth);

  std::unique_ptr<LongNode> toplevel;
  std::stack<std::reference_wrapper<LongNode>> node_stack;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH
