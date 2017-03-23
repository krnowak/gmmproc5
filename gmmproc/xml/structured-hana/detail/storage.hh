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
          typename StorageHanaTypeP>
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
template <typename StorageTagP,
          typename PartsP,
          typename StorageHanaTypeP>
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

template <typename StorageTagP,
          typename PartsP>
constexpr auto
resolve_storage_tag (StorageTagP storage_tag,
                     PartsP part_hana_types)
{
  auto contained_hana_types {hana::unpack
    (part_hana_types,
     [](auto... part_hana_type)
     {
       return hana::make_tuple
         (API::Convenience::get_type
           (hana::traits::declval (part_hana_type))...);
     })};
  auto storage_hana_type {hana::unpack
    (contained_hana_types,
     [storage_tag](auto... contained_hana_type)
     {
       using API::get_storage_type;

       return get_storage_type (storage_tag,
                                contained_hana_type...);
     })};
  auto storage_container_type {NoADL::get_storage_container_type (storage_hana_type)};

  return ResolvedStorageTag {storage_tag,
                             part_hana_types,
                             storage_container_type};
}

template <typename GettersP,
          typename IndexP>
class AccessKeyInfo
{
public:
  using Getters = GettersP;
  using Index = IndexP;

  Getters getters;
  Index index;
};

// explicitly specified deduction guide
template <typename GettersP,
          typename IndexP>
AccessKeyInfo(GettersP, IndexP) -> AccessKeyInfo<GettersP, IndexP>;

template <typename StorageHanaTypeP,
          typename ResolvedAccessInfoP>
class ContainerInfo
{
public:
  using StorageHanaType = StorageHanaTypeP;
  using ResolvedAccessInfo = ResolvedAccessInfoP;

  StorageHanaType storage_hana_type;
  ResolvedAccessInfo resolved_access_info;
};

// explicitly specified deduction guide
template <typename StorageHanaTypeP,
          typename ResolvedAccessInfoP>
ContainerInfo (StorageHanaTypeP, ResolvedAccessInfoP) -> ContainerInfo<StorageHanaTypeP, ResolvedAccessInfoP>;

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

template <typename GetterTagsTupleP,
          typename ResolvedAccessInfoTupleP,
          typename IndexP>
class GettersInfoData
{
public:
  using GetterTagsTuple = GettersTagsTupleP;
  using ResolvedAccessInfoTuple = ResolvedAccessInfoTupleP;
  using Index = IndexP;

  GetterTagsTuple getter_tags;
  ResolvedAccessInfoTuple resolved_access_infos;
  Index index;

  template <typename NewGetterTagsTupleP,
            typename NewResolvedAccessInfoTupleP>
  constexpr auto
  update_data (NewGetterTagsTupleP new_getter_tags,
               NewResolvedAccessInfoTupleP new_resolved_access_infos) const
  {
    return GettersInfoData {new_getter_tags,
                            new_resolved_access_infos,
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
template <typename GetterTagsTupleP,
          typename ResolvedAccessInfoTupleP,
          typename IndexP>
GettersInfoData (GetterTagsTupleP, ResolvedAccessInfoTupleP, IndexP) -> GettersInfoData<GetterTagsTupleP, ResolvedAccessInfoTupleP, IndexP>;

template <typename ResolvedStorageTagP>
constexpr auto
resolve_getters_info (ResolvedStorageTagP resolved_storage_tag)
{
  auto getters_info_data = hana::fold_left
    (resolved_storage_tag.parts,
     GettersInfoData {},
     [](auto getters_info_data, auto part)
     {
       auto access_info {API::Convenience::get_access_info (hana::traits::declval (part))};
       auto updated_getters_info_data = hana::fold_left
         (access_info.tuple,
          getters_info_data,
          [](auto getters_info_data,
             auto access_info_pair)
          {
            auto access_key {hana::first (access_info_pair)};
            auto getters_and_policies_tam {hana::second (access_info_pair)};

            auto updated_getter_tags {hana::concat (getters_info_data.getter_tags,
                                                    getters_and_policies_tam.keys ())};

            auto new_resolved_access_info_pair {hana::make_pair (access_key,
                                                                 AccessKeyInfo {getters_and_policies_tam,
                                                                                getters_info_data.index})};
            auto updated_resolved_access_infos {hana::append (getters_info_data.resolved_access_infos,
                                                              new_resolved_access_info_pair)};

            return getters_info_data.update_data (updated_getter_tags,
                                                  updated_resolved_access_infos);
          });

       return updated_getters_info_data.increment_index ();
     });

  auto unique_getter_tags {hana::to_set (getters_info_data.getter_tags)};
  auto resolved_access_info {NoADL::make_tuple_and_map (getters_info_data.resolved_access_infos)};

  return GettersInfo {unique_getter_tags,
                      ContainerInfo {resolved_storage_tag.storage_hana_type,
                                     resolved_access_info}};
}

template <typename ResolvedStorageTagP>
using ContainerStorageTypeT = typename ResolvedStorageTagP::StorageType::type;

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
