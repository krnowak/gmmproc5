#ifndef GMMPROC_UTILS_VIEW_HH
#define GMMPROC_UTILS_VIEW_HH

#include <memory>
#include <type_traits>
#include <utility>

namespace Gmmproc
{

namespace Utils
{

template <typename TypeP>
class View;

enum class ViewType
  {
    Mutable,
    Const
  };

namespace ViewDetails
{

template <typename TypeP, ViewType TypeV>
class Choice;

template <typename TypeP>
class Choice<TypeP, ViewType::Mutable>
{
public:
  using Type = View<TypeP>;
};

template <typename TypeP>
class Choice<TypeP, ViewType::Const>
{
public:
  using Type = View<TypeP const>;
};

} // namespace ViewDetails

template <typename TypeP, ViewType TypeV>
using ViewChoice = typename ViewDetails::Choice<TypeP, TypeV>::Type;

// TODO: Remove in favor of with std::in_place_t
class ViewInPlace {};

constexpr ViewInPlace view_in_place { };

template <typename TypeP>
class View
{
public:
  template <typename... TypesP>
  explicit View (ViewInPlace, TypesP&&... v) noexcept (std::is_nothrow_constructible<TypeP, TypesP...>())
    : wrapped {std::forward<TypesP> (v)...}
  {}

  View (View const& other) noexcept (std::is_nothrow_copy_constructible<TypeP>::value)
    : wrapped {other.wrapped}
  {}

  View (View&& other) noexcept (std::is_nothrow_move_constructible<TypeP>::value)
    : wrapped {std::move (other.wrapped)}
  {}

  ~View() noexcept (std::is_nothrow_destructible<TypeP>::value)
  {}

  View& operator= (View const& other) noexcept (std::is_nothrow_copy_constructible<TypeP>::value)
  {
    View tmp {other};

    swap (tmp);
    return *this;
  }

  View& operator= (View&& other) noexcept (std::is_nothrow_move_constructible<TypeP>::value)
  {
    View tmp {std::move (other)};

    swap (tmp);
    return *this;
  }

  void swap (View& other)
  {
    wrapped.swap (other.wrapped);
  }

  inline TypeP*
  operator-> ()
  {
    return std::addressof (wrapped);
  }

  inline TypeP const*
  operator-> () const
  {
    return std::addressof (wrapped);
  }

  inline TypeP& operator* ()
  {
    return wrapped;
  }

  inline TypeP const& operator* () const
  {
    return wrapped;
  }

private:
  friend class View<TypeP const>;

  TypeP wrapped;
};

template <typename TypeP>
class View<TypeP const>
{
public:
  template <typename... TypesP>
  explicit View (ViewInPlace, TypesP&&... v) noexcept (std::is_nothrow_constructible<TypeP, TypesP...>())
    : wrapped {std::forward<TypesP> (v)...}
  {}

  // a "copy constructor" View<Foo const> (View<Foo> const&)
  View (View<TypeP> const& other) noexcept (std::is_nothrow_copy_constructible<TypeP const>::value)
    : wrapped {other.wrapped}
  {}

  // a "move constructor" View<Foo const> (View<Foo>&&)
  View (View<TypeP>&& other) noexcept (std::is_nothrow_move_constructible<TypeP const>::value)
    : wrapped {std::move (other.wrapped)}
  {}

  ~View() noexcept (std::is_nothrow_destructible<TypeP const>::value)
  {}

  View (View const& other) noexcept (std::is_nothrow_copy_constructible<TypeP const>::value)
    : wrapped {other.wrapped}
  {}

  View (View&& other) noexcept (std::is_nothrow_move_constructible<TypeP const>::value)
    : wrapped {std::move (other.wrapped)}
  {}

  View& operator= (View const& other) = delete;
  View& operator= (View&& other) = delete;

  inline TypeP const* operator-> () const
  {
    return std::addressof (wrapped);
  }

  inline TypeP const& operator* () const
  {
    return wrapped;
  }

private:
  TypeP const wrapped;
};

template <typename TypeP>
using ViewFoo

template <typename LhsP, typename RhsP>
inline bool
operator== (View<LhsP> const& lhs,
            View<RhsP> const& rhs)
{
  return *lhs == *rhs;
}

template <typename LhsP, typename RhsP>
inline bool
operator!= (View<LhsP> const& lhs,
            View<RhsP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename LhsP, typename RhsP>
inline bool
operator== (View<LhsP> const& lhs,
            RhsP const& rhs)
{
  return *lhs == rhs;
}

template <typename LhsP, typename RhsP>
inline bool
operator!= (View<LhsP> const& lhs,
            RhsP const& rhs)
{
  return !(lhs == rhs);
}

template <typename LhsP, typename RhsP>
inline bool
operator== (LhsP const& lhs,
            View<RhsP> const& rhs)
{
  return lhs == *rhs;
}

template <typename LhsP, typename RhsP>
inline bool
operator!= (LhsP const& lhs,
            View<RhsP> const& rhs)
{
  return !(lhs == rhs);
}

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_VIEW_HH
