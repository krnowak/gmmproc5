#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_UTILS_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_UTILS_HH

#include <boost/range/iterator_range_core.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <cstddef>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

template <typename MapType>
decltype(auto)
must_get (MapType& m,
          typename MapType::key_type const& key)
{
  auto iter = m.find (key);

  if (iter == m.cend ())
  {
    std::ostringstream oss;

    oss << "no key \"" << key << "\" found in map";
    throw std::runtime_error (oss.str ());
  }
  return iter->second;
}

template <std::size_t idx, typename MapType>
auto
get_map_range (MapType& m)
{
  auto get_nth = [](auto& pair) -> decltype(auto) { return std::get<idx> (pair); };

  return boost::make_iterator_range (boost::make_transform_iterator (m.begin (), get_nth),
                                     boost::make_transform_iterator (m.end (), get_nth));
}

template <typename TypeToConst, typename TypeToCheckForConst>
class add_const_if_const
{
public:
  using Type = std::conditional_t<std::is_const<TypeToCheckForConst>::value,
                                  std::add_const_t<TypeToConst>,
                                  TypeToConst>;
};

template <typename TypeToConst, typename TypeToCheckForConst>
using add_const_if_const_t = typename add_const_if_const<TypeToConst, TypeToCheckForConst>::Type;

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_UTILS_HH
