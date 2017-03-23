#ifndef GMMPROC_XML_STRUCTURED_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_GETTERS_HH

#include <gmmproc/xml/structured/detail/storage.hh>

#include <boost/hana/at_key.hpp>
#include <boost/hana/contains.hpp>
#include <boost/hana/keys.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Getters
{

// TODO: Clarify what getters and policies mean.
//    Is it a map or tam?
//    Keys are getter hana types or getter tag hana types?
//    Also, find a better word for getters and policies.

template <typename ContainerInfoP>
class GetterRealBase : private virtual ContainerInfoP::StorageHanaType::type
{
private:
  static constexpr ContainerInfoP container_info {};

  // TODO: can be static?
  template <typename AccessKeyP>
  constexpr auto
  base_get_access_key_info (AccessKeyP access_key)
  {
    constexpr auto resolved_access_info {this->container_info.resolved_access_info};
    constexpr auto access_key_type {boost::hana::make_type (access_key)};

    static_assert (boost::hana::contains (boost::hana::keys (resolved_access_info.map),
                                          access_key),
                   "the access key must be a part of the container info");

    // TODO: Why doesn't it work?
    // return resolved_access_info.map[access_key_type];
    return boost::hana::at_key (resolved_access_info.map,
                                access_key_type);
  }

  // TODO: can be static?
  template <typename GetterTagP,
            typename AccessKeyInfoP>
  constexpr auto
  base_get_policy (GetterTagP getter_tag,
                   AccessKeyInfoP access_key_info)
  {
    constexpr auto getters {access_key_info.getters};
    constexpr auto getter_tag_type {boost::hana::make_type (getter_tag)};

    static_assert (boost::hana::contains (boost::hana::keys (getters.map),
                                          getter_tag_type),
                   "the getter tag must be associated with this access key");

    return getters.map[getter_tag_type];
  }

public:
  template <typename AccessKeyP>
  decltype(auto)
  base_get_raw (AccessKeyP access_key) const
  {
    constexpr auto access_key_info {this->base_get_access_key_info (access_key)};

    return this->storage[access_key_info.index];
  }

  template <typename GetterTagP,
            typename AccessKeyP,
            typename... PolicyArgP>
  decltype(auto)
  base_get_tagged (GetterTagP getter_tag,
                   AccessKeyP access_key,
                   PolicyArgP... policy_arg) const
  {
    constexpr auto access_key_info {this->base_get_access_key_info (access_key)};
    constexpr auto policy {this->base_get_policy (getter_tag, access_key_info)};

    return policy.get (this->storage[access_key_info.index],
                       std::forward<PolicyArgP> (policy_args)...);
  }
};

template <typename ContainerInfoP,
          typename GetterTagP>
class GetterBase : public virtual GetterRealBase<ContainerInfoP>
{
private:
  static constexpr GetterTagP getter_tag {};

protected:
  template <typename AccessKeyP,
            typename... PolicyArgP>
  decltype(auto)
  base_get (AccessKeyP access_key,
            PolicyArgP... policy_arg) const
  {
    return this->base_get_tagged (getter_tag,
                                  access_key,
                                  policy_arg...);
};

class SingleGetterTag
{};

class PassThroughPolicy
{
public:
  template <typename ValueP,
            typename... PolicyArgP>
  decltype(auto)
  get (ValueP&& value, PolicyArgP...) const
  {
    return std::forward<ValueP> (value);
  }
};

template <typename ContainerInfoP>
class SingleGetter : protected GetterBase<ContainerInfoP, SingleGetterTag>
{
public:
  template <typename AccessKeyP>
  decltype(auto)
  get (AccessKeyP access_key) const
  {
    return this->base_get (access_key);
  }
};

template <typename ContainerInfoP>
constexpr auto
get_getter_type (SingleGetterTag,
                 ContainerInfoP)
{
  return boost::hana::type_c<SingleGetter<ContainerInfoP>>;
}

} // namespace Gmmproc::Xml::Structured::Getters

#endif // GMMPROC_XML_STRUCTURED_GETTERS_HH
