#ifndef GMMPROC_XML_STRUCTURED_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_GETTERS_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/registry.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Getters
{

template <typename StorageP>
class GetterBase : protected virtual StorageP
{
private:
  template <typename AccessKeyP>
  using GetterPolicyMapT = Mpl::ApplyT<StorageP::GetGetters, AccessKeyP>;

protected:
  template <typename AccessKeyP, typename GetterP>
  using PolicyT = Mpl::AtT<GetterPolicyMapT<AccessKeyP, GetterP, void>;

  template <typename AccessKeyP, typename GetterP>
  auto const&
  base_get () const
  {
    using Policy = PolicyT<AccessKeyP, GetterP>;
    static_assert (!std::is_same<Policy, void>::value, "The Getter is associated with this AccessKeyP");
    using IndexT = Mpl::ApplyT<StorageP::GetIndex, AccessKeyP>;

    return std::get<IndexT::value> (this->storage);
  }
};

template <typename StorageP>
class SingleGetter : private virtual GetterBase<StorageP>
{
  using ThisGetter = SingleGetter<StorageP>;

  template <typename AccessKeyP>
  auto const&
  single_get () const
  {
    using Policy = PolicyT<AccessKeyP, ThisGetter>;

    return Policy::get (this->base_get<AccessKeyP, ThisGetter> ());
  }

public:
  class DefaultPolicy
  {
  public:
    template <typename ValueP>
    static auto const&
    get (ValueP const& value)
    {
      return value;
    }
  };

  template <typename AccessKeyP>
  auto const&
  get () const
  {
    return single_get<AccessKeyP> ();
  }
};

template <typename StorageP>
class VectorGetter : private virtual GetterBase<StorageP>
{
private:
  using ThisGetter = VectorGetter<StorageP>;

  template <typename AccessKeyP>
  auto const&
  vector_get () const
  {
    using Policy = PolicyT<AccessKeyP, ThisGetter>;

    return Policy::get (this->base_get<AccessKeyP, ThisGetter> ());
  }

public:
  class DefaultPolicy
  {
    template <typename ValueP>
    static auto const&
    get (ValueP const& value)
    {
      return value;
    }
  };

  template <typename AccessKeyP>
  auto const&
  get_at (std::size_t idx) const
  {
    return this->vector_get<AccessKeyP> ()[idx];
  }

  template <typename AccessKeyP>
  auto
  size () const
  {
    return this->vector_get<AccessKeyP> ().size ();
  }

  template <typename AccessKeyP>
  auto
  range () const
  {
    decltype(auto) v = this->vector_get <AccessKeyP> ();

    return boost::make_range (std::cbegin (v), std::cend (v));
  }
};

template <typename StorageP>
class MapGetter : private virtual GetterBase<StorageP>
{
private:
  using ThisGetter = MapGetter<StorageP>;

  template <typename AccessKeyP, typename AttributeKeyP>
  auto const&
  map_get () const
  {
    using Policy = PolicyT<AccessKeyP, ThisGetter>;

    return Policy::get<AttributeKeyP> (this->base_get<AccessKeyP, ThisGetter> ());
  }

public:
  // This default policy is pretty much useless, but at least it can
  // serve as an example implementation
  class DefaultPolicy
  {
    template <typename AttributeKeyP, typename ValueP>
    static auto const&
    get (ValueP const& value)
    {
      return value;
    }
  };

  template <typename AccessKeyP, typename AttributeKeyP>
  auto const&
  get_via (std::string const& name) const
  {
    using Map = Mpl::AtT<typename StorageP::TypeList,
                         Mpl::ApplyT<typename StorageP::GetIndex, AccessKeyP>>;
    using MappedType = typename Map::mapped_type;
    using Optional = std::experimental::optional<MappedType>;

    decltype(auto) m = this->map_get<AccessKeyP, AttributeKeyP> ();
    auto iter = m.find (name);

    if (iter != m.cend ())
    {
      return Optional {iter->second};
    }

    return Optional {};
  }
};

} // namespace Getters

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_GETTERS_HH
