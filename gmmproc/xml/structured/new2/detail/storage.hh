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

template <typename NodeSchemaP>
class Storage
{
private:
  using ThisStorage = Storage<NodeSchemaP>;
  using PrivHelper = StorageHelper<NodeSchemaP>;

  class AppendAccessKeysFromAccessInfo
  {
  public:
    template <typename VectorP, typename AccessInfoPairP>
    class Apply
    {
    private:
      using AccessKey = Mpl::FirstT<AccessInfoPair>;

    public:
      using Type = Mpl::InsertT<VectorP, AccessKeyP>;
    };
  };

  class AppendAccessKeysFromPart
  {
  public:
    template <typename VectorP, typename PartP>
    class Apply
    {
    private:
      using AccessInfo = Mpl::ApplyT<typename PartP::GetAccessInfo, ThisStorage>;

    public:
      using Type = Mpl::FoldT<AccessInfo,
                              VectorP,
                              AppendGetterAndIndexForAccessKey>;
    };
  };

  class AppendGetterAndIndexForAccessKey
  {
  public:
    template <typename MapP, typename AccessInfoPairP>
    class Apply
    {
    private:
      using AccessKey = Mpl::FirstT<AccessInfoPair>;
      using GettersMap = Mpl::SecondT<AccessInfoPair>;
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
    template <typename MapP, typename PartP>
    class Apply
    {
    private:
      using AccessInfo = Mpl::ApplyT<typename PartP::GetAccessInfo, ThisStorage>;

    public:
      using Type = Mpl::FoldT<AccessInfo,
                              MapP,
                              AppendGetterAndIndexForAccessKey>;
    };
  };

  using AccessKeyList = Mpl::FoldT<typename PrivHelper::PartList,
                                   Mpl::Vector<>,
                                   AppendAccessKeysFromPart>;
  using PartMap = Mpl::FoldT<typename PrivHelper::PartList,
                             Mpl::Map<>,
                             AppendAccessInfoWithIndex>;
  static_assert (Mpl::SizeT<PartMap>::value == Mpl::SizeT<AccessKeyList>::value,
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
