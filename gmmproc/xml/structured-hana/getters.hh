#ifndef GMMPROC_XML_STRUCTURED_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_GETTERS_HH

#include <gmmproc/xml/structured/detail/storage.hh>

#include <boost/hana/contains.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/keys.hpp>

namespace Gmmproc::Xml::Structured::Getters
{

template <typename NodeInfoP, typename GetterTagP>
class GetterBase : private virtual NodeInfoP::StorageHanaType::type
{
private:
  static constexpr NodeInfoP node_info {};
  static constexpr boost::hana::type<GetterTagP> getter_tag_type {};
  /*
  template <typename AccessKeyP>
  static constexpr decltype(auto)
  get_access_key_info(AccessKeyP access_key)
  {
    constexpr decltype(auto) resolved_access_info {node_info.resolved_access_info.map};
    constexpr decltype(auto) access_key_info {resolved_access_info[access_key]};

    return access_key_info;
  }
  */

protected:
  /*
  template <typename AccessKeyP, typename GetterTagP>
  static constexpr decltype(auto)
  get_policy (AccessKeyP access_key, GetterTagP getter_tag)
  {
    constexpr decltype(auto) access_key_info {get_access_key_info (access_key)};
    constexpr decltype(auto) getters {access_key_info.getters.map};
    constexpr decltype(auto) policy {getters[getter_tag]};

    return policy;
  }
  */

  template <typename AccessKeyP, typename... PolicyArgP>
  decltype(auto)
  base_get (AccessKeyP access_key,
            PolicyArgP... policy_args) const
  {
    constexpr auto resolved_access_info {this->node_info.resolved_access_info};
    constexpr auto access_key_type {boost::hana::make_type (access_key)};

    static_assert (boost::hana::contains (boost::hana::keys (resolved_access_info.map),
                                          access_key_type),
                   "the access key must be a part of the node info");

    constexpr auto access_key_info {resolved_access_info.map[access_key_type]};
    constexpr auto getters {access_key_info.getters};

    static_assert (boost::hana::contains (boost::hana::keys (getters.map),
                                          this->getter_tag_type),
                   "the getter tag must be associated with this access key");

    constexpr auto index {access_key_info.index};
    constexpr auto policy {getters.map[getter_tag_type]};

    return policy.get (this->storage[index], std::forward<PolicyArgP> (policy_args)...);
  }
};

class SingleGetterTag
{};

class PassThroughPolicy
{
public:
  template <typename ValueP, typename... PolicyArgP>
  decltype(auto)
  get (ValueP&& value, PolicyArgP...)
  {
    return std::forward<ValueP> (value);
  }
};

template <typename NodeInfoP>
class SingleGetter : private virtual GetterBase<NodeInfoP, SingleGetterTag>
{
public:
  template <typename AccessKeyP>
  decltype(auto)
  get (AccessKeyP access_key) const
  {
    return this->base_get (access_key);
  }
};

template <typename NodeInfoP>
constexpr auto
get_getter (SingleGetterTag, NodeInfoP)
{
  return boost::hana::type_c<SingleGetter<NodeInfoP>>;
}

} // namespace Gmmproc::Xml::Structured::Getters

#endif // GMMPROC_XML_STRUCTURED_GETTERS_HH
