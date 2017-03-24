#ifndef GMMPROC_XML_STRUCTURED_GROUP_HH
#define GMMPROC_XML_STRUCTURED_GROUP_HH

#include <gmmproc/xml/structured/detail/container.hh>
#include <gmmproc/xml/structured/detail/group.hh>
#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>
#include <gmmproc/xml/structured/part.hh>

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured
{

class GroupTag
{};

template <typename AccessKeyP,
          typename StorageTagP,
          typename... PartP>
class Group : public Part<GroupTag>
{
public:
  using AccessKey = AccessKeyP;
  using StorageTag = StorageTagP;
  using Parts = boost::hana::tuple<PartP...>;

  AccessKey access_key;
  StorageTag storage_tag;
  Parts parts;
};

template <typename StorageTagP,
          typename PartsP>
class GroupImpl : private virtual Detail::ContainerStorageT<StorageTagP, PartsP>,
                  public Detail::ContainerStorageT<StorageTagP, PartsP>
{
public:
  template <typename AccessKeyP,
            typename SubPolicyP>
  class ForwardingPolicy
  {
  public:
    template <typename... PolicyArgP>
    decltype(auto)
    get (GroupImpl&& group_impl,
         PolicyArgP... policy_arg) const
    {
      constexpr AccessKeyP access_key {};
      constexpr SubPolicyP subpolicy {};

      return subpolicy.get (group_impl.base_get_raw (access_key),
                            policy_arg...);
    }
  };
};

template <typename GroupTypeP>
constexpr auto
get_type (GroupTag,
          GroupTypeP)
{
  return hana::type_c<GroupImpl<typename GroupTypeP::type::StorageTag,
                                typename GroupTypeP::type::Parts>>;
}

template <typename GroupTypeP>
constexpr auto
get_access_info (GroupTag,
                 GroupTypeP group_type)
{
  auto group = boost::hana::traits::declval (group_type);
  auto sub_access_infos = Detail::get_sub_access_infos (group);

  auto main_getter_tag_and_policy_pair = boost::hana::make_pair (Getters::SingleGetterTag {},
                                                                 Getters::PassThroughPolicy {});
  auto main_getter_tags_and_policies {Detail::make_tuple_and_map
    (boost::hana::make_tuple (main_getter_tag_and_policy_pair))};
  auto main_access_info_pair {boost::hana::make_pair (group.access_key,
                                                      main_getter_tags_and_policies)};
  auto main_access_info {Detail::make_tuple_and_map
    (boost::hana::make_tuple (main_access_info_pair))};
  auto all_access_infos {boost::hana::prepend (sub_access_infos,
                                               main_access_info)};

  return Detail::merge_exclusive_tuple_and_maps (all_access_infos);
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH
