#ifndef MM_UTILS_ITERATOR_WRAPPER_HH
#define MM_UTILS_ITERATOR_WRAPPER_HH

#include <experimental/optional>

namespace Mm
{

namespace Utils
{

namespace IteratorWrapperDetails
{

// TODO: These will need to be moved out from this namespace.
// T is simply a type
template <typename It, typename T, typename = void>
class W
{
public:
  using Type = T;

  auto
  get (typename It::value_type const& v)
  {
    return Type{v};
  }
};

// T is a callable type
template <typename It, typename T>
class W<It, T, std::result_of_t<T(It::value_type const&)> >
{
public:
  using Type = std::result_of_t<T(It::value_type const&)>;

  W(T f)
    : func {f}
  {}

  auto
  get (typename It::value_type const& v)
  {
    return func(v);
  }
private:
  T func;
};

} // namespace IteratorWrapperDetails


template <typename Iterator, typename WrapperType, typename = void>
class IteratorWrapper
{
public:
  using difference_type = typename Iterator::difference_type;
  using value_type = Wrapper;
  using pointer = Wrapper*;
  using reference = Wrapper&;

  using iterator_category = typename Iterator::iterator_category;
};


template <typename Iterator, typename Converter>
class IteratorWrapper
{
  using Wrapper = typename Converter::Type;

public:
  using difference_type = typename Iterator::difference_type;
  using value_type = Wrapper;
  using pointer = Wrapper*;
  using reference = Wrapper&;

  using iterator_category = typename Iterator::iterator_category;

  IteratorWrapper (Converter c)
    : t {{}, {}, c}
  {}

  IteratorWrapper (Iterator wrapped, Converter c)
    : t {wrapped, {}, c}
  {}

  bool operator==(IteratorWrapper const& rhs) const
  {
    return std::get<0> (t) == std::get<0> (rhs.t);
  }

  bool operator!=(IteratorWrapper const& rhs) const
  {
    return std::get<0> (t) != std::get<0> (rhs.t);
  }

  reference operator*() const
  {
    std::get<1> (t) = std::get<2> (t).get (*std::get<0> (t));
    return *w;
  }

  pointer operator->() const
  {
    std::get<1> (t) = std::get<2> (t).get (*std::get<0> (t));
    return &*w;
  }

  const IteratorWrapper& operator++()
  {
    ++std::get<0> (t);
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
    --std::get<0> (t);
    return *this;
  }

  IteratorWrapper operator--(int)
  {
    auto temp = *this;
    --*this;

    return temp;
  }

private:
  using Opt = std::experimental::optional<value_type>;

  mutable std::tuple<Iterator, Opt, Converter> t;
};

} // namespace Utils

} // namespace Mm

#endif // MM_UTILS_ITERATOR_WRAPPER_HH
