#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH

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

class Long
{
public:
  struct Counted
  {
    std::size_t count = 0;
  };

  enum class Leaf
  {
    UNDETERMINED,
    NEVER_A_LEAF,
    SOMETIMES_A_LEAF,
    ALWAYS_A_LEAF
  };

  struct Leafed
  {
    Leaf leaf = Leaf::UNDETERMINED;
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

  struct Child : public Named, public Counted, public Leafed
  {
    using Named::Named;

    std::size_t min_occurences = std::numeric_limits<std::size_t>::max ();
    std::size_t max_occurences = 0;
  };

  struct Node : public Named, public Counted, public Leafed
  {
    using Named::Named;

    NamedSet<Attribute> attributes;
    NamedSet<Child> children;
    bool has_text = false;
  };

  Long() = default;

  void
  process_node (pugi::xml_node const& node);

  void
  process_document (pugi::xml_node const& document);

  NamedSet<Node>&&
  steal ();

private:
  NamedSet<Node> nodes;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_LONG_HH
