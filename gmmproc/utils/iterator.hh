#ifndef GMMPROC_UTILS_ITERATOR_HH
#define GMMPROC_UTILS_ITERATOR_HH

#include <boost/iterator/filter_iterator.hpp>

namespace Gmmproc
{

namespace Utils
{

template <typename PredicateP, typename RangeP>
auto
make_filter_range (PredicateP pred, RangeP& range)
{
  auto b = range.begin ();
  auto e = range.end ();
  auto fb = boost::make_filter_iterator (pred, b, e);
  auto fe = boost::make_filter_iterator (pred, e, e);
  return boost::make_iterator_range (fb, fe);
}

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_ITERATOR_HH
