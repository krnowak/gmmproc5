#ifndef GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/detail/storage.hh>
#include <gmmproc/xml/structured/getters.hh>

#include <tuple>
#include <type_traits>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename GetterP>
using GetterWithDefaultPolicy = Mpl::Map<Mpl::Pair<GetterP, typename GetterP::DefaultPolicy>>;

template <typename StorageP>
using SingleGetterWithDefaultPolicy = GetterWithDefaultPolicy<Getters::SingleGetter<StorageP>>;

template <typename... PartP>
class Getters
{
private:
  using StorageType = Storage<PartP...>;

  class GetGettersFromAccessInfoEntry
  {
  public:
    template <typename SequenceP, typename AccessInfoEntryP>
    class Apply
    {
    private:
      using GettersMap = Mpl::SecondT<AccessInfoEntryP>;
      using GettersVector = Mpl::TransformT<GettersMap, typename boost::mpl::lambda<Mpl::First<Mpl::_1>>::type, Mpl::VectorBackInserter>;

    public:
      using Type = Mpl::InsertRangeT<SequenceP, Mpl::EndT<SequenceP>, GettersVector>;
    };
  };

  class GetUniqueGetters
  {
  public:
    template <typename SetP, typename OnePartP>
    class Apply
    {
    private:
      using AccessInfo = Mpl::ApplyT<typename OnePartP::GetAccessInfo,
                                     StorageType>;
      using GettersVector = Mpl::FoldT<AccessInfo,
                                       Mpl::Vector<>,
                                       GetGettersFromAccessInfoEntry>;

    public:
      using Type = Mpl::InsertRangeT<SetP, Mpl::BeginT<SetP>, GettersVector>;
    };
  };

  using UniqueGetters = Mpl::FoldT<Mpl::Vector<PartP...>,
                                   Mpl::Set<>,
                                   GetUniqueGetters>;
  using GettersSubclass = Mpl::InheritSequenceT<UniqueGetters>;

public:
  using Type = GettersSubclass;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH
