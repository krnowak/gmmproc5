#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH

#include <mm/xml/base/xml.hh>

#include <limits>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <cstdint>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

using Str = std::string;
using StrSet = std::unordered_set<Str>;
template <typename T>
using StrMap = std::unordered_map<Str, T>;
using StrQueue = std::queue<Str>;

struct Named
{
  Named (Str n)
    : name {std::move (n)}
  {}

  Str name;
};

struct Counted
{
  std::size_t count = 0;
};

struct WithOccurences
{
  std::size_t min_occurences = std::numeric_limits<std::size_t>::max ();
  std::size_t max_occurences = 0;
};

enum class LeafType
{
  UNDETERMINED,
  NEVER_A_LEAF,
  SOMETIMES_A_LEAF,
  ALWAYS_A_LEAF
};

struct Leafed
{
  LeafType is_leaf = LeafType::UNDETERMINED;
};

class Attribute : public Named
{
public:
  class Impl;

  AttributeType (Str const& name);

  void
  update (Str const& value);

  Str
  to_string () const;

private:
  std::unique_ptr<Impl> impl;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH
