#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/detail/type-helpers.hh>
#include <gmmproc/xml/structured/detail/storage-helper.hh>

#include <type_traits>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename... ChildrenP>
class Storage
{
private:
  using ThisStorage = Storage<ChildrenP...>;
  using PrivHelper = StorageHelper<ChildrenP...>;

  template <typename OffsetP = Utils::StdSizeTConstant<0>>
  class AppendGetterAndIndexForAccessKey
  {
  public:
    template <typename MapP, typename PartP>
    class Apply
    {
    private:
      using GettersMap = Mpl::ApplyT<typename PartP::GetGetters, ThisStorage>;
      using Index = Utils::StdSizeTConstant<Mpl::SizeT<MapP>::value + OffsetP::value>;
      using Pair = Mpl::Pair<GettersMap, Index>;
      using AccessKey = Mpl::ApplyT<typename PartP::GetAccessKey>;
      using KeyValue = Mpl::Pair<AccessKey, Pair>;

    public:
      using Type = Mpl::InsertT<MapP, KeyValue>;
    };
  };

  using PartMap = Mpl::FoldT<typename PrivHelper::PartList,
                             Mpl::Map<>,
                             AppendGetterAndIndexForAccessKey<>>;
  static_assert (Mpl::SizeT<PartMap>::value == Mpl::SizeT<typename PrivHelper::PartList>,
                 "all access keys are unique");

  template <typename PairMemberP>
  class QueryPair
  {
    template <typename AccessKeyP>
    class Apply
    {
    private:
      using Pair = Mpl::AtT<PartMap, AccessKeyP, void>;
      static_assert (!std::is_same<Pair, void>::value, "access key is in the storage");

    public:
      using Type = typename PairMemberP<Pair>::Type;
    };
  };

protected:
  using GetGetters = QueryPair<Mpl::First>;
  using GetIndex = QueryPair<Mpl::Second>;
  using Helper = PrivHelper;

  typename Helper::TupleType storage;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
