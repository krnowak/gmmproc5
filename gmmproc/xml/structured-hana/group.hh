#ifndef GMMPROC_XML_STRUCTURED_GROUP_HH
#define GMMPROC_XML_STRUCTURED_GROUP_HH

#include <gmmproc/xml/structured/detail/container.hh>
#include <gmmproc/xml/structured/detail/group.hh>
#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/part.hh>

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured
{

class GroupTag {};

template <typename AccessKeyP,
          typename StorageTagP,
          typename... PartP>
class Group : public Part<GroupTag>
{
public:
  using AccessKey = AccessKeyP;
  using StorageTag = StorageTagP;
  using Parts = boost::hana::tuple<boost::hana::type<PartP>...>;

  AccessKey access_key;
  StorageTag storage_tag;
};

template <typename StorageTagP,
          typename PartsP>
class GroupImpl : public Detail::ContainerT<StorageTagP, PartsP>
{
};

template <typename GroupP>
constexpr auto
get_type (GroupTag,
          GroupP)
{
  return hana::type_c<GroupImpl<typename GroupP::StorageTag, typename GroupP::Parts>>;
}

template <typename GroupP>
constexpr auto
get_access_info (GroupTag,
                 GroupP group)
{
  auto access_infos = boost::hana::unpack
    (group.parts,
     [](auto... part_hana_type)
     {
       return boost::hana::make_tuple
       (APICall::get_access_info (hana::traits::declval (part_hana_type))...);
     });
  auto single_pair = boost::hana::make_pair (Getters::SingleGetterTag {},
                                             Getters::PassThroughPolicy {});

  auto group_getter {Detail::make_tuple_and_map (boost::hana::make_tuple (single_pair))};
  auto access_info_pair {boost::hana::make_pair (group.access_key, group_getter)};
  auto group_access_info {Detail::make_tuple_and_map (boost::hana::make_tuple (access_info_pair))};
  auto all_access_infos {boost::hana::append (access_infos,
                                              group_access_info)};

  return Detail::merge_exclusive_tuple_and_maps (all_access_infos);
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH
