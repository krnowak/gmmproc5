#ifndef GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH

#include <gmmproc/utils/integral-types.hh>
#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/registry.hh>

#include <experimental/optional>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

class GetPartType
{
public:
  template <typename PartP>
  class Apply
  {
  public:
    using Type = Mpl::ApplyT<typename PartP::GetContainedType>;
  };
};

template <typename PartListP>
using GetPartTypeListT = Mpl::TransformT<PartListP, GetPartType, Mpl::VectorBackInserter>;

class GetContainedTypeFromPair
{
public:
  template <typename BasicAndNodeTagPairP>
  class Apply
  {
  private:
    using Basic = typename BasicAndNodeTagPairP::first;
    using NodeTag = typename BasicAndNodeTagPairP::second;

  public:
    using Type = Mpl::ApplyT<typename Basic::GetContainedType, NodeTag>;
  };
};

template <typename BasicAndNodeTagPairListP>
using GetContainedTypesT = Mpl::TransformT<BasicAndNodeTagPairListP,
                                           GetContainedTypeFromPair,
                                           Mpl::VectorBackInserter>;

class GetUniquePtr
{
public:
  template <typename UnderlyingP>
  class Apply
  {
  public:
    using Type = std::unique_ptr<UnderlyingP>;
  };
};

class GetOptional
{
public:
  template <typename UnderlyingP>
  class Apply
  {
  public:
    using Type = std::experimental::optional<UnderlyingP>;
  };
};

template <OptionalType ValueP>
using OptConstant = std::integral_constant<OptionalType, ValueP>;

class GetOptionalType
{
private:
  using Map = Mpl::Map<Mpl::Pair<OptConstant<OptionalType::Pointer>, GetUniquePtr>,
                       Mpl::Pair<OptConstant<OptionalType::Value>, GetOptional>>;

public:
  template <OptionalType ValueP, typename UnderlyingP>
  class Apply
  {
  private:
    using TypeGetter = Mpl::AtT<Map, ValueP, void>;
    static_assert (!std::is_same<TypeGetter, void>::value, "the OptionalType value is valid");

  public:
    using Type = Mpl::ApplyT<TypeGetter, UnderlyingP>;
  };
}

template <typename TupleP, typename TypeP>
struct AppendToStdTuple;

template <typename... TupleTypesP, typename TypeP>
struct AppendToStdTuple<std::tuple<TupleTypesP...>, TypeP>
{
  using Type = std::tuple< TupleTypesP..., TypeP >;
};

template <typename ListP>
using GetStdTupleTypeT = Mpl::FoldT<ListP, std::tuple<>, AppendToStdTuple<Mpl::_1, Mpl::_2>>;

class GetMapVectorTupleType
{
public:
  template <typename AttrKeysP, typename NodeTagP>
  class Apply
  {
  private:
    using NodeType = typename Registry::NodeInfo<NodeTagP>::Type;
    using VectorType = std::vector<NodeType>;
    using Ref = std::reference_wrapper<NodeType>;
    using MapType = std::unordered_map<std::string, Ref>;
    using Size = Mpl::SizeT<AttrKeysP>;
    using FinalVector = Mpl::FoldT<Mpl::StdSizeTRange<Size>,
                                   Mpl::Vector<VectorType>,
                                   Mpl::PushBack<Mpl::_1, MapType>
                                   >;

  public:
    using Type = GetStdTupleTypeT<FinalVector>;
  };
};

class Flatten
{
public:
  template <typename SequenceP, typename OtherSequenceP>
  class Apply
  {
  public:
    using Type = Mpl::InsertRangeT<SequenceP, Mpl::EndT<SequenceP>, OtherSequenceP>;
  };
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH
