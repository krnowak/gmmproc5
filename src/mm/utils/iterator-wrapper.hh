#ifndef MM_UTILS_ITERATOR_WRAPPER_HH
#define MM_UTILS_ITERATOR_WRAPPER_HH

#include <experimental/optional>

namespace Mm
{

namespace Utils
{

template <typename Iterator, typename Wrapper>
class IteratorWrapper
{
public:
  using difference_type = typename Iterator::difference_type;
  using value_type = Wrapper;
  using pointer = Wrapper*;
  using reference = Wrapper&;

  using iterator_category = typename Iterator::iterator_category;

  IteratorWrapper ()
    : it {},
      w {}
  {}

  IteratorWrapper (Iterator wrapped)
    : it {wrapped},
      w {}
  {}

  bool operator==(IteratorWrapper const& rhs) const
  {
    return it == rhs.it;
  }

  bool operator!=(IteratorWrapper const& rhs) const
  {
    return it != rhs.it;
  }

  reference operator*() const
  {
    w = Wrapper {*it};
    return *w;
  }

  pointer operator->() const
  {
    w = Wrapper {*it};
    return &*w;
  }

  const IteratorWrapper& operator++()
  {
    ++it;
    return *this;
  }

  IteratorWrapper operator++(int)
  {
    auto temp = *this;
    ++*this;

    return temp;
  }

  const IteratorWrapper& operator--()
  {
    --it;
    return *this;
  }

  IteratorWrapper operator--(int)
  {
    auto temp = *this;
    --*this;

    return temp;
  }

private:
  Iterator it;
  mutable std::experimental::optional<Wrapper> w;
};

} // namespace Utils

} // namespace Mm

#endif // MM_UTILS_ITERATOR_WRAPPER_HH
