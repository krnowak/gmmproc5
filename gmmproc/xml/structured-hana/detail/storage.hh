#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>

#include <boost/hana/append.hpp>
#include <boost/hana/concat.hpp>
#include <boost/hana/contains.hpp>
#include <boost/hana/erase_key.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/fold_left.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/keys.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/set.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/zip_with.hpp>

#include <boost/hana/ext/std/integer_sequence.hpp>

#include <type_traits>
#include <utility>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename PartP>
class StorageMemberBase
{
protected:
  static constexpr PartP part;

  using Type = typename decltype(API::Convenience::get_type (part))::type;
};

class <typename PartP, PartKind PartKindV = API::Convenience::get_part_kind (std::declval<PartP> ())>
class StorageMember;

template <typename PartP>
class StorageMember<PartP, PartKind::Normal> : private StorageMemberBase<PartP>
{
private:
  using Base = StorageMemberBase<PartP>;

protected:
  template <typename StructuredTagP,
            typename... InputP>
  StorageMember(StructuredTagP structured_tag,
                InputP&&... input)
    : member {NoADL::generate_member (structured_tag, Base::part, std::forward<InputP> (input)...)}
  {}

  Base::Type member;
};

template <typename PartP>
class StorageMember<PartP, PartKind::Extra> : private StorageMemberBase<PartP>
{
private:
  using Base = StorageMemberBase<PartP>;

protected:
  template <typename StructuredTagP,
            typename... InputP>
  StorageMember(StructuredTagP,
                InputP&&...)
    : member {}
  {}

  Base::Type member;
};

template <typename PartP>
class StorageMember<PartP, PartKind::NonMember> : private StorageMemberBase<PartP>
{
private:
  using Base = StorageMemberBase<PartP>;

protected:
  template <typename StructuredTagP,
            typename... InputP>
  StorageMember(StructuredTagP structured_tag,
                InputP&&... input)
  {
    NoADL::generate_member (structured_tag, Base::part, std::forward<InputP> (input)...);
  }
};

template <typename... PartP>
class Storage : private StorageMember<PartP>...
{
private:
  using ThisStorage = Storage<PartP...>;
  using Members = hana::tuple<hana::type<StorageMember<PartP>...>>;

  static constexpr Members members;

public:
  using Parts = hana::tuple<PartP...>;

  static constexpr Parts parts;

  template <typename StructuredTag,
            typename... InputP>
  Storage (StructuredTag structured_tag,
           InputP... input)
    : StorageMember<PartP> {structured_tag,
                            input...}...
  {}

  // TODO: deduplicate (differs only with constness)
  // TODO: make sure we return the reference to const
  // TODO: assert that the part is not a NonMember kind
  template <typename IndexP>
  decltype(auto)
  operator[] (IndexP index) const
  {
    constexpr auto member_type {ThisStorage::members[index]};
    using MemberType = decltype(member_type)::type;

    return this->MemberType::member;
  }

  template <typename IndexP>
  decltype(auto)
  operator[] (IndexP index)
  {
    constexpr auto member_type {ThisStorage::members[index]};
    using MemberType = decltype(member_type)::type;

    return this->MemberType::member;
  }
};

template <typename... PartP>
get_storage_type_from_parameter_pack (PartP...)
{
  return hana::type_c<Storage<PartP...>>;
}

template <typename PartsP>
constexpr auto
get_storage_type (PartsP parts)
{
  return hana::unpack (parts,
                       [](auto... part)
                       {
                         return get_storage_type_from_parameter_pack (part...);
                       });
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

template <typename PartsP>
constexpr auto
resolve_getters_info (PartsP parts)
{
  auto getters_info_data = hana::fold_left
    (parts,
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
  auto storage_type {NoADL::get_storage_type (parts)};

  return GettersInfo {unique_getter_tags,
                      ContainerInfo {storage_type,
                                     resolved_access_info}};
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
