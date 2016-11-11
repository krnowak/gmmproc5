#ifndef GMMPROC_XML_STRUCTURED_DETAIL_NODE_INFO_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_NODE_INFO_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <boost/hana/append.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/fold_left.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/tuple.hpp>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename ResolvedStorageTagP, typename AccessKeyP>
constexpr auto
get_getters (ResolvedStorageTagP resolved_tag, AccessKeyP access_key)
{
  auto parts_info = NoADL::get_parts_info (resolved_tag);
  auto getters_and_index = parts_info.map[access_key];

  return getters_and_index[0_c];
}

template <typename ResolvedStorageTagP, typename AccessKeyP>
constexpr auto
get_index (ResolvedStorageTagP resolved_tag, AccessKeyP access_key)
{
  auto parts_info = NoADL::get_parts_info (resolved_tag);
  auto getters_and_index = parts_info.map[access_key];

  return getters_and_index[1_c];
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_NODE_INFO_HH
