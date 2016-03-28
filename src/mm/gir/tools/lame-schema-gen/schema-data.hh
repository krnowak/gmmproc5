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

struct ShortNode : public Named, public Counted
{
  struct Child : public Named, public WithOccurences, public Counted
  {
    using Named::Named;
  };

  using ChildMap = StrMap<Child>;
  using Sets = std::vector<StrSortedSet>;

  using Named::Named;

  StrMap<Attribute> attributes;
  Sets children_sets;
  StrSortedSet exclusives;
  StrSortedSet common;
  StrMap<bool> parents;
  ChildMap children;
  bool has_text = false;
  std::size_t count_as_leaf = 0;
  std::size_t count_as_nonleaf = 0;
};

class GirPaths;

class SchemaData
{
public:
  static SchemaData
  get_from_gir_paths (GirPaths const& paths);

  StrSet names;
  StrMap<ShortNode> short_data;

private:
  SchemaData () = default;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_SCHEMA_DATA_HH
