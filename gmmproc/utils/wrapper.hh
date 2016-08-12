#ifndef GMMPROC_UTILS_WRAPPER_HH
#define GMMPROC_UTILS_WRAPPER_HH

#include <memory>
#include <type_traits>
#include <utility>

namespace Gmmproc
{

namespace Utils
{

// TODO: Remove in favor of with std::in_place_t
class WrapperInPlace {};

constexpr WrapperInPlace wrapper_in_place { };

template <typename TypeP>
class Wrapper
{
public:
  template <typename... TypesP>
  explicit Wrapper (WrapperInPlace, TypesP&&... v) noexcept (std::is_nothrow_constructible<TypeP, TypesP...>())
    : wrapped {std::forward<TypesP> (v)...}
  {}

  Wrapper (Wrapper const& other) noexcept (std::is_nothrow_copy_constructible<TypeP>::value)
    : wrapped {other.wrapped}
  {}

  Wrapper (Wrapper&& other) noexcept (std::is_nothrow_move_constructible<TypeP>::value)
    : wrapped {std::move (other.wrapped)}
  {}

  ~Wrapper() noexcept (std::is_nothrow_destructible<TypeP>::value)
  {}

  Wrapper& operator= (Wrapper const& other) noexcept (std::is_nothrow_copy_constructible<TypeP>::value)
  {
    Wrapper tmp {other};

    swap (tmp);
    return *this;
  }

  Wrapper& operator= (Wrapper&& other) noexcept (std::is_nothrow_move_constructible<TypeP>::value)
  {
    Wrapper tmp {std::move (other)};

    swap (tmp);
    return *this;
  }

  void swap (Wrapper& other)
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

  bool equal (Wrapper const& other) const
  {
    return wrapped == other.wrapped;
  }

  bool equal (Wrapper<TypeP const> const& other) const
  {
    return other == *this;
  }

private:
  friend class Wrapper<TypeP const>;

  TypeP wrapped;
};

template <typename TypeP>
class Wrapper<TypeP const>
{
public:
  template <typename... TypesP>
  explicit Wrapper (WrapperInPlace, TypesP&&... v) noexcept (std::is_nothrow_constructible<TypeP, TypesP...>())
    : wrapped {std::forward<TypesP> (v)...}
  {}

  // a "copy constructor" Wrapper<Foo const> (Wrapper<Foo> const&)
  Wrapper (Wrapper<TypeP> const& other) noexcept (std::is_nothrow_copy_constructible<TypeP const>::value)
    : wrapped {other.wrapped}
  {}

  // a "move constructor" Wrapper<Foo const> (Wrapper<Foo>&&)
  Wrapper (Wrapper<TypeP>&& other) noexcept (std::is_nothrow_move_constructible<TypeP const>::value)
    : wrapped {std::move (other.wrapped)}
  {}

  ~Wrapper() noexcept (std::is_nothrow_destructible<TypeP const>::value)
  {}

  Wrapper (Wrapper const& other) noexcept (std::is_nothrow_copy_constructible<TypeP const>::value)
    : wrapped {other.wrapped}
  {}

  Wrapper (Wrapper&& other) noexcept (std::is_nothrow_move_constructible<TypeP const>::value)
    : wrapped {std::move (other.wrapped)}
  {}

  Wrapper& operator= (Wrapper const& other) = delete;
  Wrapper& operator= (Wrapper&& other) = delete;

  inline TypeP const* operator-> () const
  {
    return std::addressof (wrapped);
  }

  inline TypeP const& operator* () const
  {
    return wrapped;
  }

  bool equal (Wrapper const& other) const
  {
    return wrapped == other.wrapped;
  }

  bool equal (Wrapper<TypeP> const& other) const
  {
    return wrapped == other.wrapped;
  }

private:
  TypeP const wrapped;
};

template <typename TypeP>
inline bool
operator== (Wrapper<TypeP> const& lhs,
            Wrapper<TypeP> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename TypeP>
inline bool
operator== (Wrapper<TypeP const> const& lhs,
            Wrapper<TypeP> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename TypeP>
inline bool
operator== (Wrapper<TypeP> const& lhs,
            Wrapper<TypeP const> const& rhs)
{
  return lhs.equal (rhs);
}

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_WRAPPER_HH
