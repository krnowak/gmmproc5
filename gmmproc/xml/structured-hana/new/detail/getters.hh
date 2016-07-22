#ifndef GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
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

class MapVectorVectorPolicy
{
public:
  template <typename ValueP>
  static auto const&
  get (ValueP const& value)
  {
    return std::get <0> (value);
  }
};

template <typename AttributeKeysP>
class MapVectorMapPolicy
{
  template <typename AttributeKeyP, typename ValueP>
  static auto const&
  get (ValueP const& value)
  {
    using Iterator = Mpl::FindT<AttributeKeysP, AttributeKeyP>;
    using Begin = Mpl::BeginT<AttributeKeysP>;
    using End = Mpl::EndT<AttributeKeysP>;
    static_assert (!std::is_same<Iterator, End>::value, "given AttributeKey exists in the child");

    return std::get<Mpl::DistanceT<Begin, Iterator>::value + 1> (value);
  }
};

template <typename NodeSchemaP>
class Getters
{
private:
  using Helper = StorageHelper<NodeSchemaP>;
  using StorageType = Storage<NodeSchemaP>;

  class GetUniqueGetters
  {
  public:
    template <typename MapP, typename PartP>
    class Apply
    {
    private:
      using GettersMap = Mpl::ApplyT<typename PartP::GetGetters, StorageType>;
      using GettersSet = Mpl::TransformT<GettersMap, Mpl::First<Mpl::_1>, Mpl::SetInserter>;

    public:
      using Type = Mpl::InsertRangeT<MapP, Mpl::BeginT<MapP>, GettersSet>;;
    };
  };

  using AllGetters = Mpl::FoldT<typename Helper::PartList,
                                Mpl::Set<>,
                                GetUniqueGetters>;
  using GettersSubclass = Mpl::InheritSequenceT<AllGetters>;

public:
  using Type = GettersSubclass;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_GETTERS_HH
