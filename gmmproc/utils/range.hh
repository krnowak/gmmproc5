#ifndef GMMPROC_UTILS_RANGE_HH
#define GMMPROC_UTILS_RANGE_HH

#include <utility>

namespace Gmmproc
{

namespace Utils
{

template <typename Iterator>
class Range
{
public:
  using iterator = Iterator;
  using const_iterator = Iterator const;

  Range(iterator bi, iterator ei)
    : b {std::move (bi)},
      e {std::move (ei)}
  {}

  iterator begin () const
  {
    return b;
  }

  iterator end () const
  {
    return e;
  }

private:
  iterator b;
  iterator e;
};

// TODO: write begin(), cbegin(), end(), cend() as functions for Range

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_RANGE_HH