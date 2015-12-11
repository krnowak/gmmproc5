#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH

#include "base.hh"
#include "types.hh"

#include <limits>
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

class Long : public Base
{
public:
  struct Counted
  {
    std::size_t count = 0;
  };

  struct Attribute : public Named, public Counted
  {
    enum class Type
    {
      UNDETERMINED,
      NUMERIC,
      STRING
    };

    using Named::Named;

    Type type = Type::UNDETERMINED;
  };

  struct Node : public Named, public Counted
  {
    using Named::Named;

    StrMap<Attribute> attributes;
    StrMap<Node> children;
    std::size_t min_occurences = std::numeric_limits<std::size_t>::max ();
    std::size_t max_occurences = 0;
    bool has_text = false;
    int depth = 0;
  };

  std::unique_ptr<Node>&&
  steal ();

private:
  void
  virtual process_node_vfunc(std::string const& name, pugi::xml_node const& node, int depth) override;

  void
  process_toplevel (std::string const& name, pugi::xml_node const& node, int depth);

  void
  process_element (std::string const& name, pugi::xml_node const& node, int depth);

  void
  setup_node_stack_for_depth (int depth)

  std::unique_ptr<Node> toplevel;
  std::stack<Node&> node_stack;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH
