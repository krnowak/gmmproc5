#ifndef GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH
#define GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH

#include <limits>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstddef>

namespace Gmmproc
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

using Str = std::string;
using StrSet = std::unordered_set<Str>;
using StrSortedSet = std::set<Str>;
using StrMultiSet = std::unordered_multiset<Str>;
template <typename T>
using StrMap = std::unordered_map<Str, T>;
using StrQueue = std::queue<Str>;
using StrVector = std::vector<Str>;

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
  // how many parents have at least one child with this name
  std::size_t parent_count = 0;
};

class Attribute : public Named, public Counted
{
public:
  class Type;

  Attribute (Str const& name);
  Attribute (Attribute&& attribute);
  Attribute (Attribute const& attribute) = delete;
  ~Attribute ();

  Attribute& operator= (Attribute&& attribute);
  Attribute& operator= (Attribute const& attribute) = delete;

  void
  update (Str const& value);

  Str
  to_string () const;

  bool
  is_unique () const;

  void
  mark_as_common ();

private:
  std::unique_ptr<Type> impl;
  bool unique;
};

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Gmmproc

#endif // GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH
