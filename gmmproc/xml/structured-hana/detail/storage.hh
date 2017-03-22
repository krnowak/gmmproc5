#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>
#include <gmmproc/xml/structured/apicall.hh>

#include <boost/hana/append.hpp>
#include <boost/hana/concat.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/fold_left.hpp>
#include <boost/hana/insert_range.hpp>
#include <boost/hana/keys.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/set.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/unpack.hpp>

#include <type_traits>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename StorageTagP, typename PartsP, typename StorageHanaTypeP>
class ResolvedStorageTag
{
public:
  using StorageTag = StorageTagP;
  using Parts = PartsP;
  using StorageHanaType = StorageHanaTypeP;

  StorageTag storage_tag;
  Parts parts;
  StorageHanaType storage_hana_type;
};

// explicitly specified deduction guide
template <typename StorageTagP, typename PartsP, typename StorageHanaTypeP>
ResolvedStorageTag(StorageTagP, PartsP, StorageHanaTypeP) -> ResolvedStorageTag<StorageTagP, PartsP, StorageHanaTypeP>;

template <typename StorageImplP>
class StorageContainer
{
public:
  template <typename... TypeP>
  StorageContainer (TypeP... args)
    : storage {std::forward<TypeP> (args)...}
  {}

protected:
  StorageImplP storage;
};

template <typename StorageImplHanaTypeP>
constexpr auto
get_storage_container_type (StorageImplHanaTypeP)
{
  return hana::type_c<StorageContainer<typename StorageImplHanaTypeP::type>>;
}

template <typename StorageTagP, typename PartsP>
constexpr auto
resolve_storage_tag (StorageTagP storage_tag, PartsP part_hana_types)
{
  auto contained_hana_types = hana::unpack
    (part_hana_types,
     [](auto... part_hana_type)
     {
       return hana::make_tuple
       (APICall::get_type
           (hana::traits::declval (part_hana_type))...);
     });
  auto storage_hana_type = hana::unpack
    (contained_hana_types,
     [storage_tag](auto... contained_hana_type)
     {
       using API::get_storage_type;

       return get_storage_type (storage_tag, contained_hana_type...);
     });
  auto storage_container_type = NoADL::get_storage_container_type (storage_hana_type);

  return ResolvedStorageTag {storage_tag, part_hana_types, storage_container_type};
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

// explicitly specified deduction guide
template <typename GettersP, typename IndexP>
AccessKeyInfo(GettersP, IndexP) -> AccessKeyInfo<GettersP, IndexP>;

template <typename StorageHanaTypeP, typename ResolvedAccessInfoP>
class NodeInfo
{
public:
  using StorageHanaType = StorageHanaTypeP;
  using ResolvedAccessInfo = ResolvedAccessInfoP;

  StorageHanaType storage_hana_type;
  ResolvedAccessInfo resolved_access_info;
};

// explicitly specified deduction guide
template <typename StorageHanaTypeP, typename ResolvedAccessInfoP>
NodeInfo(StorageHanaTypeP, ResolvedAccessInfoP) -> NodeInfo<StorageHanaTypeP, ResolvedAccessInfoP>;

template <typename UniqueGetterTagsP, typename NodeInfoP>
class GettersInfo
{
public:
  using UniqueGetterTags = UniqueGetterTagsP;
  using NodeInfo = NodeInfoP;

  UniqueGetterTags unique_getter_tags;
  NodeInfo node_info;
};

// explicitly specified deduction guide
template <typename UniqueGetterTagsP, typename NodeInfoP>
GettersInfo(UniqueGetterTagsP, NodeInfoP) -> GettersInfo<UniqueGetterTagsP, NodeInfoP>;

template <typename ResolvedStorageTagP>
constexpr auto
resolve_getters_info (ResolvedStorageTagP resolved_storage_tag)
{
  auto tags_tuple_and_resolved_access_info_tuple = hana::fold_left
    (resolved_storage_tag.parts,
     hana::make_pair (hana::make_tuple (), hana::make_tuple ()),
     [](auto tags_tuple_and_resolved_access_info_tuple, auto part)
     {
       auto access_info {APICall::get_access_info (hana::traits::declval (part))};

       return hana::fold_left
         (access_info.tuple,
          tags_tuple_and_resolved_access_info_tuple,
          [](auto tags_tuple_and_resolved_access_info_tuple, auto access_info_pair)
          {
            auto tags_tuple = hana::first (tags_tuple_and_resolved_access_info_tuple);
            auto getters = hana::second (access_info_pair);
            auto keys_tuple {hana::to_tuple (hana::keys (getters.map))};
            auto new_tags_tuple = hana::concat (tags_tuple, keys_tuple);

            auto resolved_access_info_tuple = hana::second (tags_tuple_and_resolved_access_info_tuple);
            auto access_key = hana::first (access_info_pair);
            auto index = hana::length (resolved_access_info_tuple);
            auto new_resolved_access_info_tuple = hana::append (resolved_access_info_tuple,
                                                                hana::make_pair (access_key,
                                                                                 AccessKeyInfo {getters, index}));

            return hana::make_pair (new_tags_tuple,
                                    new_resolved_access_info_tuple);
          });
     });

  auto getter_tags_tuple = hana::first (tags_tuple_and_resolved_access_info_tuple);
  auto unique_getter_tags = hana::to_set (getter_tags_tuple);
  auto resolved_access_info_tuple = hana::second (tags_tuple_and_resolved_access_info_tuple);
  auto resolved_access_info = NoADL::make_tuple_and_map (resolved_access_info_tuple);
  auto node_info {NodeInfo {resolved_storage_tag.storage_hana_type, resolved_access_info}};

  return GettersInfo {unique_getter_tags, node_info};
}

template <typename ResolvedStorageTagP>
using NodeStorageTypeT = typename ResolvedStorageTagP::StorageType::type;

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
