#ifndef GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_UTILS_HH
#define GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_UTILS_HH

#include <boost/range/iterator_range_core.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <cstddef>

namespace Gmmproc
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

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

template <typename MapType>
add_const_if_const_t<typename MapType::mapped_type, MapType>&
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

template <std::size_t idx, typename MapTypeIterator>
auto
get_map_iterator (MapTypeIterator&& i)
{
  auto get_nth = [](auto& pair) -> decltype(auto) { return std::get<idx> (pair); };

  return boost::make_transform_iterator (std::forward<MapTypeIterator> (i), get_nth);
}

template <std::size_t idx, typename MapType>
auto
get_map_range (MapType& m)
{

  return boost::make_iterator_range (get_map_iterator<idx> (m.begin ()),
                                     get_map_iterator<idx> (m.end ()));
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Gmmproc

#endif // GMMPROC_GIR_TOOLS_LAME_SCHEMA_GEN_UTILS_HH