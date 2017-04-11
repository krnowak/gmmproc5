#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>

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

template <typename StorageTagP,
          typename PartsP,
          typename StorageTypeP>
class ResolvedStorageTag
{
public:
  using StorageTag = StorageTagP;
  using Parts = PartsP;
  using StorageType = StorageTypeP;

  StorageTag storage_tag;
  Parts parts;
  StorageType storage_type;
};

// explicitly specified deduction guide
template <typename StorageTagP,
          typename PartsP,
          typename StorageTypeP>
ResolvedStorageTag(StorageTagP, PartsP, StorageTypeP) -> ResolvedStorageTag<StorageTagP, PartsP, StorageTypeP>;

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

template <typename StorageImplTypeP>
constexpr auto
get_storage_container_type (StorageImplTypeP)
{
  return hana::type_c<StorageContainer<typename StorageImplTypeP::type>>;
}

template <typename StorageTagP,
          typename PartsP>
constexpr auto
resolve_storage_tag (StorageTagP storage_tag,
                     PartsP parts)
{
  auto contained_types {hana::unpack
    (parts,
     [](auto... part)
     {
       return hana::make_tuple (API::Convenience::get_type (part)...);
     })};
  auto storage_type = NoADL::get_storage_type (some_structured_tag,
                                               contained_types);
  auto storage_container_type {NoADL::get_storage_container_type (storage_type)};

  // TODO: check how parts in resolved storage tag are used - as a
  // tuple of parts or as a tuple of part types
  return ResolvedStorageTag {storage_tag,
                             parts,
                             storage_container_type};
}

// access key info

template <typename GetterTagsAndPoliciesP,
          typename IndexP>
class AccessKeyInfo
{
public:
  using GetterTagsAndPolicies = GetterTagsAndPoliciesP;
  using Index = IndexP;

  GetterTagsAndPolicies getter_tags_and_policies;
  Index index;
};

// explicitly specified deduction guide
template <typename GetterTagsAndPoliciesP,
          typename IndexP>
AccessKeyInfo(GetterTagsAndPoliciesP, IndexP) -> AccessKeyInfo<GetterTagsAndPoliciesP, IndexP>;

// container info

template <typename StorageTypeP,
          typename ResolvedAccessInfoP>
class ContainerInfo
{
public:
  using StorageType = StorageTypeP;
  using ResolvedAccessInfo = ResolvedAccessInfoP;

  StorageType storage_type;
  ResolvedAccessInfo resolved_access_info;
};

// explicitly specified deduction guide
template <typename StorageTypeP,
          typename ResolvedAccessInfoP>
ContainerInfo (StorageTypeP, ResolvedAccessInfoP) -> ContainerInfo<StorageTypeP, ResolvedAccessInfoP>;

// getters info

template <typename UniqueGetterTagsP,
          typename ContainerInfoP>
class GettersInfo
{
public:
  using UniqueGetterTags = UniqueGetterTagsP;
  using ContainerInfo = ContainerInfoP;

  UniqueGetterTags unique_getter_tags;
  ContainerInfo container_info;
};

// explicitly specified deduction guide
template <typename UniqueGetterTagsP,
          typename ContainerInfoP>
GettersInfo (UniqueGetterTagsP, ContainerInfoP) -> GettersInfo<UniqueGetterTagsP, ContainerInfoP>;

// getters info data

template <typename GetterTagsP,
          typename ResolvedAccessInfoP,
          typename IndexP>
class GettersInfoData
{
public:
  using GetterTags = GettersTagsP;
  using ResolvedAccessInfos = ResolvedAccessInfosP;
  using Index = IndexP;

  GetterTags getter_tags;
  ResolvedAccessInfos resolved_access_infos;
  Index index;

  template <typename UpdatedGetterTagsP,
            typename UpdatedResolvedAccessInfosP>
  constexpr auto
  update_data (UpdatedGetterTagsP updated_getter_tags,
               UpdatedResolvedAccessInfosP updated_resolved_access_infos) const
  {
    return GettersInfoData {updated_getter_tags,
                            updated_resolved_access_infos,
                            this->index};
  }

  constexpr auto
  increment_index () const
  {
    return GettersInfoData {this->getter_tags,
                            this->resolved_access_infos,
                            this->index + 1};
  }
};

// explicitly specified deduction guides
template <>
GettersInfoData () -> GettersInfoData<hana::tuple<>, hana::tuple<>, hana::size_t<0>>;
template <typename GetterTagsP,
          typename ResolvedAccessInfosP,
          typename IndexP>
GettersInfoData (GetterTagsP, ResolvedAccessInfosP, IndexP) -> GettersInfoData<GetterTagsP, ResolvedAccessInfosP, IndexP>;

// stable uniq data

template <typename ValuesP,
          typename ValuesSetP>
class StableUniqData
{
public:
  using Values = ValuesP;
  using ValuesSet = ValuesSetP;

  Values values;
  ValuesSet values_set;
};

// explicitly specified deduction guides
template <typename ValuesSetP>
StableUniqData () -> StableUniqData<hana::tuple<>, ValuesSetP>;
template <typename ValuesP,
          typename ValuesSetP>
StableUniqData (ValuesP, ValuesSetP) -> StableUniqData<ValuesP, ValuesSetP>;

template <typename ValuesP>
constexpr auto
stable_uniq (ValuesP values)
{
  auto values_set {hana::to_set (values)};
  auto stable_uniq_data {hana::fold_left
    (values,
     StableUniqData {values_set},
     [](auto stable_uniq_data,
        auto value)
     {
       if constexpr (hana::contains (stable_uniq_data.values_set,
                                     value))
       {
         auto updated_values = hana::append (stable_uniq_data.values,
                                             value);
         auto updated_values_set = hana::erase_key (stable_uniq_data.values_set,
                                                    value);

         return StableUniqData {updated_values,
                                updated_values_set};
       }
       else
       {
         return stable_uniq_data;
       }
     })};

  static_assert (hana::length (stable_uniq_data.values_set) == hana::size_c<0>,
                 "initial helper set of values should be empty after deduplication");

  return stable_uniq_data.values;
}

template <typename ResolvedStorageTagP>
constexpr auto
resolve_getters_info (ResolvedStorageTagP resolved_storage_tag)
{
  auto getters_info_data = hana::fold_left
    (resolved_storage_tag.parts,
     GettersInfoData {},
     [](auto getters_info_data, auto part)
     {
       auto access_info {API::Convenience::get_access_info (part)};
       auto updated_getters_info_data = hana::fold_left
         (access_info.tuple,
          getters_info_data,
          [](auto getters_info_data,
             auto access_info_pair)
          {
            auto access_key {hana::first (access_info_pair)};
            auto getters_tags_and_policies {hana::second (access_info_pair)};

            auto getter_tag_types {hana::keys (getters_tags_and_policies.map)};
            auto updated_getter_tag_types {hana::concat (getters_info_data.getter_tags,
                                                         getter_tag_types)};

            auto access_key_info {AccessKeyInfo {getters_tags_and_policies,
                                                 getters_info_data.index}};
            auto new_resolved_access_info_pair {hana::make_pair (access_key,
                                                                 access_key_info)};
            auto updated_resolved_access_infos {hana::append (getters_info_data.resolved_access_infos,
                                                              new_resolved_access_info_pair)};

            return getters_info_data.update_data (updated_getter_tags,
                                                  updated_resolved_access_infos);
          });

       return updated_getters_info_data.increment_index ();
     });

  auto unique_getter_tag_types {NoADL::stable_uniq (getters_info_data.getter_tags)};
  auto unique_getter_tags {hana::unpack
    (unique_getter_tag_types,
     [](auto... getter_tag)
     {
       return hana::make_tuple (hana::traits::declval (getter_tag)...);
     })};
  auto resolved_access_info {NoADL::make_tuple_and_map (getters_info_data.resolved_access_infos)};

  return GettersInfo {unique_getter_tags,
                      ContainerInfo {resolved_storage_tag.storage_type,
                                     resolved_access_info}};
}

template <typename ResolvedStorageTagP>
using ContainerStorageTypeT = typename ResolvedStorageTagP::StorageType::type;

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
