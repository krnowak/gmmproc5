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
          typename... PartP>
class Group : public Part<GroupTag>
{
public:
  using AccessKey = AccessKeyP;
  using Parts = boost::hana::tuple<PartP...>;

  AccessKey access_key;
  Parts parts;
};

template <typename PartsP>
class GroupImpl : private virtual Detail::ContainerStorageT<PartsP>,
                  public Detail::ContainerStorageT<PartsP>
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
  return hana::type_c<GroupImpl<typename GroupTypeP::type::Parts>>;
}

template <typename GroupTypeP>
constexpr auto
get_access_info (GroupTag,
                 GroupTypeP group_type)
{
  auto group = boost::hana::traits::declval (group_type);
  auto sub_access_infos = Detail::get_sub_access_infos (group);
  auto main_access_info {Detail::get_simple_access_info (group.access_key,
                                                         Getters::SingleGetterTag {},
                                                         Getters::PassThroughPolicy {})};
  auto all_access_infos {boost::hana::prepend (sub_access_infos,
                                               main_access_info)};

  return Detail::merge_exclusive_tuple_and_maps (all_access_infos);
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH
