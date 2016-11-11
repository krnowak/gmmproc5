#ifndef GMMPROC_XML_STRUCTURED_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_GETTERS_HH

#include <tuple>

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/registry.hh>

namespace Gmmproc::Xml::Structured::Getters
{

template <typename NodeInfoP>
class GetterBase : private virtual NodeInfoP::ResolvedStorageTag::StorageHanaType::type
{
private:
  static constexpr NodeInfoP node_info;

  template <typename AccessKeyP>
  static constexpr decltype(auto)
  get_access_key_info(AccessKeyP access_key)
  {
    constexpr auto resolved_access_info {node_info.resolved_access_info.map};

    return resolved_access_info[access_key];
  }

protected:
  template <typename AccessKeyP, typename GetterP>
  static constexpr auto
  get_policy (AccessKeyP access_key, hana::type<GetterP> getter_type)
  {
    auto access_key_info = get_access_key_info (access_key);
    auto getters {access_key_info.getters.map};

    return getters[getter_type];
  }

  template <typename AccessKeyP, typename GetterP>
  auto const&
  base_get () const
  {
    constexpr auto getter {hana::type_c<GetterP>};
    constexpr auto access_key_info = get_access_key_info (std::declval<AccessKeyP> ());
    constexpr auto getters {access_key_info.getters.map};

    static_assert (hana::contains (getters, getter),
                   "the getter should be associated with this access key");

    constexpr auto index {access_key_info.index};

    return this->storage[index];
  }
};

class SingleGetterTag
{};

template <typename NodeInfoP>
constexpr auto
get_getter (SingleGetterTag, NodeInfoP)
{
  return hana::type_c<SingleGetter<NodeInfoP>>;
}

class SingleGetterDefaultPolicy
{
public:
  template <typename ValueP>
  static decltype(auto)
  get (ValueP const& value)
  {
    return value;
  }
};

template <typename NodeInfoP>
class SingleGetter : private virtual GetterBase<NodeInfoP>
{
  using ThisGetter = SingleGetter<StorageP>;
  using Base = GetterBase<StorageP>;

  template <typename AccessKeyP>
  decltype(auto)
  single_get (AccessKeyP access_key) const
  {
    constexpr auto policy = this->get_policy (access_key);
    using Policy = typename Base::template PolicyT<AccessKeyP, ThisGetter>;

    return Policy::get (this->template base_get<AccessKeyP, ThisGetter> ());
  }

public:
  template <typename AccessKeyP>
  decltype(auto)
  get(AccessKeyP access_key) const
  {
    constexpr auto policy = this->get_policy (access_key);
    using Policy = typename Base::template PolicyT<AccessKeyP, ThisGetter>;

    return Policy::get (this->template base_get<AccessKeyP, ThisGetter> ());
  }
};

} // namespace Gmmproc::Xml::Structured::Getters

#endif // GMMPROC_XML_STRUCTURED_GETTERS_HH
