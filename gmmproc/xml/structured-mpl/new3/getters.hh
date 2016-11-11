#ifndef GMMPROC_XML_STRUCTURED_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_GETTERS_HH

#include <tuple>

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
class GetterBase : private virtual StorageP
{
private:
  using StorageHelper = typename Detail::GetStorageHelper<StorageP>::Type;
  template <typename AccessKeyP>
  using GettersMap = Detail::Mpl::ApplyT<typename StorageHelper::GetGettersMap,
                                         AccessKeyP>;

protected:
  template <typename AccessKeyP, typename GetterP>
  using PolicyT = Detail::Mpl::AtT<GettersMap<AccessKeyP>,
                                   GetterP>;

  template <typename AccessKeyP, typename GetterP>
  auto const&
  base_get () const
  {
    using Policy = PolicyT<AccessKeyP,
                           GetterP>;
    static_assert (!std::is_same<Policy, Detail::Mpl::Void>::value,
                   "the getter should be associated with this access key");
    using IndexT = Detail::Mpl::ApplyT<typename StorageHelper::GetIndex,
                                       AccessKeyP>;

    return std::get<IndexT::value> (this->storage);
  }
};

template <typename StorageP>
class SingleGetter : private virtual GetterBase<StorageP>
{
  using ThisGetter = SingleGetter<StorageP>;
  using Base = GetterBase<StorageP>;

  template <typename AccessKeyP>
  auto const&
  single_get () const
  {
    using Policy = typename Base::template PolicyT<AccessKeyP, ThisGetter>;

    return Policy::get (this->template base_get<AccessKeyP, ThisGetter> ());
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

} // namespace Getters

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_GETTERS_HH
