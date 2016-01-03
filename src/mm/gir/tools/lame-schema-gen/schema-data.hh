#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DATA_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DATA_HH

#include "types.hh"

#include <memory>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

struct ShortNode : public Named, public Counted, public Leafed
{
  struct Child : public Named, public Counted, public Leafed, public WithOccurences
  {
    using Named::Named;
  };

  using Named::Named;

  StrMap<Attribute> attributes;
  StrMap<Child> children;
  bool has_text = false;
};

struct LongNode : public Named, public Counted, public WithOccurences
{
  using Named::Named;

  StrMap<Attribute> attributes;
  StrSet unique_attributes;
  StrSet common_attributes;
  StrMap<std::unique_ptr<LongNode>> children;
  bool has_text = false;
  int depth = 0;
};

class GirPaths;

class SchemaData
{
public:
  static SchemaData
  get_from_gir_paths (GirPaths const& paths);

  StrSet names;
  StrMap<ShortNode> short_data;
  std::unique_ptr<LongNode> toplevel_long_node;

private:
  SchemaData () = default;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DATA_HH
