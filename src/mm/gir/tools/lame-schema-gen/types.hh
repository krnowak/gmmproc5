#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH

#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace pugi
{

class xml_node;

} // namespace pugi

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

namespace LameSchemaGenDetails
{

template <typename T>
struct Hash
{
  using StrH = std::hash<std::string>;

public:
  size_t
  operator() (T const& d) const noexcept
  {
    return StrH {}(d.name);
  }
};

template <typename T>
struct EqualTo
{
  using StrE = std::equal_to<std::string>;

public:
  bool
  operator() (T const& a, T const& b) const
  {
    return StrE {}(a.name, b.name);
  }
};

} // namespace LameSchemaGenDetails

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

template <typename T>
using NamedSet = std::unordered_set<T,
                                    LameSchemaGenDetails::Hash<T>,
                                    LameSchemaGenDetails::EqualTo<T>>;

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_TYPES_HH
