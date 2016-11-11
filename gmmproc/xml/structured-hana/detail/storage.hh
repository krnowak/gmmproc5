#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <boost/hana/append.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/fold_left.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/unpack.hpp>

#include <type_traits>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename ResolvedStorageTagP, typename ResolvedAccessInfoP>
class NodeInfo
{
public:
  using ResolvedStorageTag = ResolvedStorageTagP;
  using ResolvedAccessInfo = ResolvedAccessInfoP;

  ResolvedStorageTag resolved_storage_tag;
  ResolvedAccessInfo resolved_access_info;
};

template <typename StorageTagP, typename PartsP>
constexpr auto
resolve_node (StorageTagP tag, PartsP parts)
{
  auto resolved_storage_tag = NoADL::resolve_storage_tag (tag, parts);
  auto resolved_access_info = NoADL::resolve_access_info (resolved_storage_tag);

  return NodeInfo {resolved_storage_tag, resolved_access_info};
}

template <typename StorageTagP, typename StorageHanaTypeP, typename PartsP>
class ResolvedStorageTag
{
public:
  using StorageTag = StorageTagP;
  using Parts = PartsP;
  using StorageHanaType = StorageHanaTypeP;

  static_assert (std::is_trivial_v<StorageTag>);
  //static_assert (std::conjunction_v<std::is_trivial_v<Parts>...>);
  static_assert (std::is_trivial_v<StorageTag>);
  static_assert (std::is_trivial_v<ResolvedStorageTag<StorageTag, Parts, StorageHanaType>>);

  StorageTag storage_tag;
  Parts parts;
  StorageHanaType storage_type;
};

template <typename StorageTagP, typename PartsP>
constexpr auto
resolve_storage_tag (StorageTagP tag, PartsP parts)
{
  auto contained_hana_types = hana::unpack
    (parts,
     [](auto... part)
     {
       return hana::make_tuple (get_contained_type (part)...);
     });
  auto storage_type = hana::unpack
    (contained_hana_types,
     [storage_tag](auto... contained_hana_type)
     {
       return get_storage_type (storage_tag, contained_hana_type...);
     });

  return ResolvedStorageTag {tag, storage_type, parts};
}

template <typename GettersP, typename IndexP>
class AccessKeyInfo
{
public:
  using Getters = GettersP;
  using Index = IndexP;

  Getters getters;
  Index index;
};

template <typename ResolvedStorageTagP>
constexpr auto
resolve_access_info (ResolvedStorageTagP resolved_storage_tag)
{
  return NoADL::make_tuple_and_map
    (hana::fold_left
     (resolved_storage_tag.parts,
      hana::make_tuple (),
      [resolved_storage_tag](auto&& parts_info_tuple, auto&& part)
      {
        auto access_info = get_access_info (part, resolved_storage_tag);

        return hana::fold_left
          (access_info.tuple,
           parts_info_tuple,
           [](auto&& parts_info_tuple, auto&& access_info_pair)
           {
             auto access_key = hana::first (access_info_pair);
             auto getters = hana::second (access_info_pair);
             auto index = hana::length (parts_info_tuple);

             return hana::append (parts_info_tuple,
                                  hana::make_pair (access_key,
                                                   AccessKeyInfo {getters, index}));
           });
      }));
}

template <typename StorageTagP, typename PartP...>
using ResolveStorageTagT = decltype(NoADL::resolve_storage_tag (StorageTagP {},
                                                                hana::tuple_t<PartP...>));

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
