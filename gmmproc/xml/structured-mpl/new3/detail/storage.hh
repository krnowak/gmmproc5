#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/detail/type-helpers.hh>

#include <gmmproc/utils/integral-types.hh>

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

template <typename... PartP>
class Storage
{
protected:
  using TupleType = std::tuple<Mpl::ApplyT<typename PartP::GetContainedType>...>;

  TupleType storage;
};

template <typename... PartP>
class StorageHelper
{
private:
  using ThisStorage = Storage<PartP...>;
  using PartsVector = Mpl::Vector<PartP...>;

  class AppendAccessKeyFromAccessInfoEntry
  {
  public:
    template <typename VectorP, typename AccessInfoEntryP>
    class Apply
    {
    private:
      using AccessKey = Mpl::FirstT<AccessInfoEntryP>;

    public:
      using Type = Mpl::InsertT<VectorP,
                                AccessKey>;
    };
  };

  class AppendAccessKeysFromPart
  {
  public:
    template <typename VectorP, typename OnePartP>
    class Apply
    {
    private:
      using AccessInfo = Mpl::ApplyT<typename OnePartP::GetAccessInfo,
                                     ThisStorage>;

    public:
      using Type = Mpl::FoldT<AccessInfo,
                              VectorP,
                              AppendAccessKeyFromAccessInfoEntry>;
    };
  };

  class AppendGettersMapAndIndexForAccessKey
  {
  public:
    template <typename MapP, typename AccessInfoEntryP>
    class Apply
    {
    private:
      using AccessKey = Mpl::FirstT<AccessInfoEntryP>;
      using GettersMap = Mpl::SecondT<AccessInfoEntryP>;
      using Index = Utils::StdSizeTConstant<Mpl::SizeT<MapP>::value>;
      using Pair = Mpl::Pair<GettersMap, Index>;
      using KeyValue = Mpl::Pair<AccessKey, Pair>;

    public:
      using Type = Mpl::InsertT<MapP, KeyValue>;
    };
  };

  class AppendAccessInfoWithIndex
  {
  public:
    template <typename MapP, typename OnePartP>
    class Apply
    {
    private:
      using AccessInfo = Mpl::ApplyT<typename OnePartP::GetAccessInfo,
                                     ThisStorage>;

    public:
      using Type = Mpl::FoldT<AccessInfo,
                              MapP,
                              AppendGettersMapAndIndexForAccessKey>;
    };
  };

  using AccessKeyList = Mpl::FoldT<PartsVector,
                                   Mpl::Vector<>,
                                   AppendAccessKeysFromPart>;
  // maps access key to pair of a getters map (getter -> policy) and
  // an index in the tuple
  using PartMap = Mpl::FoldT<PartsVector,
                             Mpl::Map<>,
                             AppendAccessInfoWithIndex>;
  static_assert (Mpl::SizeT<PartMap>::value == Mpl::SizeT<AccessKeyList>::value,
                 "all access keys should be unique");

  template <typename PairMemberP>
  class QueryPair
  {
    template <typename AccessKeyP>
    class Apply
    {
    private:
      using Pair = Mpl::AtT<PartMap,
                            AccessKeyP>;
      constexpr static auto FoundV = std::is_same<Pair,
                                                  Mpl::Void>::value;
      static_assert (!FoundV, "access key should be in the storage");

    public:
      using Type = Mpl::ApplyT<PairMemberP, Pair>;
    };
  };

public:
  using GetGettersMap = QueryPair<Mpl::First<Mpl::_1>>;
  using GetIndex = QueryPair<Mpl::Second<Mpl::_1>>;
};

template <typename StorageP>
class GetStorageHelper;

template<typename... PartP>
class GetStorageHelper<Storage<PartP...>>
{
public:
  using Type = StorageHelper<PartP...>;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
