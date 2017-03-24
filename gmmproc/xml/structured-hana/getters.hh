#ifndef GMMPROC_XML_STRUCTURED_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_GETTERS_HH

#include <gmmproc/xml/structured/detail/storage.hh>

#include <boost/hana/at_key.hpp>
#include <boost/hana/contains.hpp>
#include <boost/hana/keys.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured
{

template <typename ContainerInfoP>
class GetterRealBase : private virtual ContainerInfoP::StorageHanaType::type
{
private:
  using ThisType = GetterRealBase<ContainerInfoP>;
  static constexpr ContainerInfoP container_info {};

  template <typename AccessKeyP>
  static constexpr auto
  base_get_access_key_info (AccessKeyP access_key)
  {
    constexpr auto resolved_access_info {ThisType::container_info.resolved_access_info};
    constexpr auto access_key_type {boost::hana::make_type (access_key)};

    static_assert (boost::hana::contains (boost::hana::keys (resolved_access_info.map),
                                          access_key),
                   "the access key must be a part of the container info");

    // TODO: If this fails, uncomment the lines below instead
    return resolved_access_info.map[access_key_type];
    // return boost::hana::at_key (resolved_access_info.map,
    //                             access_key_type);
  }

  template <typename GetterTagP,
            typename AccessKeyInfoP>
  static constexpr auto
  base_get_policy (GetterTagP getter_tag,
                   AccessKeyInfoP access_key_info)
  {
    constexpr auto getter_tags_and_policies {access_key_info.getter_tags_and_policies};
    constexpr auto getter_tag_type {boost::hana::make_type (getter_tag)};

    static_assert (boost::hana::contains (boost::hana::keys (getter_tags_and_policies.map),
                                          getter_tag_type),
                   "the getter tag must be associated with this access key");

    return getter_tags_and_policies.map[getter_tag_type];
  }

  template <typename StorageP,
            typename AccessKeyP>
  static decltype(auto)
  base_get_raw_from_storage (StorageP& storage,
                             AccessKeyP access_key)
  {
    constexpr auto access_key_info {ThisType::base_get_access_key_info (access_key)};

    return storage[access_key_info.index];
  }

  template <typename StorageP,
            typename GetterTagP,
            typename AccessKeyP,
            typename... PolicyArgP>
  static decltype(auto)
  base_get_tagged_from_storage (StorageP& storage,
                                GetterTagP getter_tag,
                                AccessKeyP access_key,
                                PolicyArgP... policy_arg) const
  {
    constexpr auto access_key_info {ThisType::base_get_access_key_info (access_key)};
    constexpr auto policy {ThisType::base_get_policy (getter_tag, access_key_info)};

    return policy.get (storage[access_key_info.index],
                       std::forward<PolicyArgP> (policy_args)...);
  }

public:
  template <typename AccessKeyP>
  decltype(auto)
  base_get_raw (AccessKeyP access_key) const
  {
    return ThisType::base_get_raw_from_storage (this->storage,
                                                access_key);
  }

  template <typename AccessKeyP>
  decltype(auto)
  base_get_raw (AccessKeyP access_key)
  {
    return ThisType::base_get_raw_from_storage (this->storage,
                                                access_key);
  }

  template <typename GetterTagP,
            typename AccessKeyP,
            typename... PolicyArgP>
  decltype(auto)
  base_get_tagged (GetterTagP getter_tag,
                   AccessKeyP access_key,
                   PolicyArgP... policy_arg) const
  {
    return ThisType::base_get_tagged_from_storage (this->storage,
                                                   getter_tag,
                                                   access_key,
                                                   policy_arg...);
  }

  template <typename GetterTagP,
            typename AccessKeyP,
            typename... PolicyArgP>
  decltype(auto)
  base_get_tagged (GetterTagP getter_tag,
                   AccessKeyP access_key,
                   PolicyArgP... policy_arg)
  {
    return ThisType::base_get_tagged_from_storage (this->storage,
                                                   getter_tag,
                                                   access_key,
                                                   policy_arg...);
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
  }

  template <typename AccessKeyP,
            typename... PolicyArgP>
  decltype(auto)
  base_get (AccessKeyP access_key,
            PolicyArgP... policy_arg)
  {
    return this->base_get_tagged (getter_tag,
                                  access_key,
                                  policy_arg...);
  }
};

class PassThroughPolicy
{
public:
  template <typename ValueP,
            typename... PolicyArgP>
  decltype(auto)
  get (ValueP&& value,
       PolicyArgP...) const
  {
    return std::forward<ValueP> (value);
  }
};

// single

class SingleGetterTag
{};

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

  template <typename AccessKeyP>
  decltype(auto)
  get (AccessKeyP access_key)
  {
    return this->base_get (access_key);
  }
};

template <typename ContainerInfoTypeP>
constexpr auto
get_getter_type (SingleGetterTag,
                 ContainerInfoTypeP)
{
  return boost::hana::type_c<SingleGetter<typename ContainerInfoTypeP::type>>;
}

// extra data

class ExtraDataGetterTag
{};

template <typename ContainerInfoP>
class ExtraDataGetter : protected GetterBase<ContainerInfo, ExtraDataGetterTag>
{
public:
  decltype(auto)
  get_extra_data () const
  {
    return this->base_get (Detail::ExtraDataAccessKey {});
  }

  decltype(auto)
  get_extra_data ()
  {
    return this->base_get (Detail::ExtraDataAccessKey {});
  }
};

template <typename ContainerInfoTypeP>
constexpr auto
get_getter_type (ExtraDataGetterTag,
                 ContainerInfoTypeP)
{
  return boost::hana::type_c<ExtraDataGetter<typename ContainerInfoTypeP::type>>;
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_GETTERS_HH
