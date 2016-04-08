#ifndef GMMPROC_UTILS_LAMBDA_WRAPPER_HH
#define GMMPROC_UTILS_LAMBDA_WRAPPER_HH

#include <experimental/optional>

namespace Gmmproc
{

namespace Utils
{

template <typename Lambda>
struct LambdaWrapper
{
  LambdaWrapper()
    : lambda {}
  {
    throw std::logic_error ("trying to use default-constructor of lambda wrapper");
  }

  LambdaWrapper(Lambda&& l)
    : lambda {std::move (l)}
  {}

  /*

  // Assignment operator is just a copy construction, which does not provide
  // the strong exception guarantee.
  function_object& operator=(const function_object& rhs)
  {
    if (this != &rhs)
    {
      this->~function_object();
      new (this) function_object(rhs);
    }
    return *this;
  }

  */

  /* 
  template<class F>
  struct result
  {};

  template<class F, class T>
  struct result<F(T)>
  {
    typedef decltype(std::declval<Fun>()(std::declval<T>())) type;
  };

  */

  template<class T>
  auto operator() (T&& x) const
  {
    return (*lambda)(std::forward<T>(x));
  }

  template<class T>
  auto operator() (T&& x)
  {
    return (*lambda)(std::forward<T>(x));
  }

private:
  std::experimental::optional<Lambda> lambda;
};

template<class Lambda>
LambdaWrapper<Lambda> make_lambda_wrapper (Lambda l)
{
  return LambdaWrapper<Lambda>(std::move (l));
}

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_LAMBDA_WRAPPER_HH
