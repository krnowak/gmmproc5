#ifndef GMMPROC_XML_STRUCTURED_DETAIL_MPL_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_MPL_HH

#include <boost/mpl/apply.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>

#include <gmmproc/xml/structured/detail/mpl-boostify.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

namespace Mpl
{

using _1 = boost::mpl::_1;
using _2 = boost::mpl::_2;

template <typename... TypesP>
using Vector = boost::mpl::vector<TypesP...>;

template <typename FirstP, typename SecondP>
using Pair = boost::mpl::pair<FirstP, SecondP>;

template <typename... TypesP>
using Set = boost::mpl::set<TypesP...>;

template <typename... PairsP>
using Map = boost::mpl::map<PairsP...>;

template <typename SequenceP, typename PosP, typename RangeP>
using InsertRangeT = typename boost::mpl::insert_range<SequenceP, PosP, RangeP>::type;

template <typename SequenceP>
using BeginT = typename boost::mpl::template begin<SequenceP>::type;

template <typename SequenceP>
using EndT = typename boost::mpl::template end<SequenceP>::type;

template <typename SequenceP, typename StateP, typename ForwardOpP>
using FoldT = typename boost::mpl::template fold<SequenceP, StateP, BoostifyT<ForwardOpP>>::type;

template <typename SequenceP, typename OpP, typename InP>
using TransformT = typename boost::mpl::template transform<SequenceP, BoostifyT<OpP>, InP>::type;

using VectorBackInserter = boost::mpl::back_inserter<Vector<>>;
using MapInserter = boost::mpl::inserter<Map<>, boost::mpl::insert<_1, _2>>;
using SetInserter = boost::mpl::inserter<Set<>, boost::mpl::insert<_1, _2>>;

template <typename SequenceP, typename PredP>
using CountIfT = typename boost::mpl::count_if<SequenceP, BoostifyT<PredP>>::type;

template <typename IteratorP>
using DerefT = typename boost::mpl::deref<IteratorP>::type;

template <typename SequenceP>
using SizeT = typename boost::mpl::size<SequenceP>::type;

template <typename SequenceP, typename TypeP>
using PushBack = boost::mpl::push_back<SequenceP, TypeP>;

template <typename SizeP>
using StdSizeTRange = boost::mpl::range_c<std::size_t, 0, SizeP::value>;

template <typename SequenceP, typename TypeP>
using FindT = typename boost::mpl::find<SequenceP, TypeP>::type;

template <typename PairP>
class First
{
public:
  using Type = typename boost::mpl::first<PairP>::type;
};

template <typename PairP>
using FirstT = typename First<PairP>::Type;

template <typename PairP>
class Second
{
public:
  using Type = typename boost::mpl::second<PairP>::type;
};

template <typename PairP>
using SecondT = typename Second<PairP>::Type;

template <typename MetaFunctionP, typename... ArgsP>
using ApplyT = typename boost::mpl::apply<BoostifyT<MetaFunctionP>, ArgsP...>::type;

template <typename SequenceP>
using InheritSequenceT = typename boost::mpl::inherit_linearly<SequenceP,
                                                               boost::mpl::inherit<_1, _2>>::type;

template <typename SequenceP, typename IndexP>
using AtT = typename boost::mpl::template at<SequenceP, IndexP>::type;

using Void = boost::mpl::void_;

template <typename SequenceP, typename TypeP>
using InsertT = typename boost::mpl::insert<SequenceP, TypeP>::type;

} // namespace Mpl

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_MPL_HH
