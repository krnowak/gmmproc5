#ifndef GMMPROC_UTILS_EXCEPTIONS_HH
#define GMMPROC_UTILS_EXCEPTIONS_HH

#include <boost/any.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace Gmmproc
{

namespace Utils
{

class Exception
{
public:
  virtual ~Exception () noexcept = default;

  boost::iterator_range<std::vector<std::exception_ptr>::const_iterator>
  nested_exceptions () const
  {
    return boost::make_iterator_range (m_nested.cbegin (), m_nested.cend ());
  }

  boost::iterator_range<std::vector<std::string>::const_iterator>
  contexts () const
  {
    return boost::make_iterator_range (m_contexts.cbegin (), m_contexts.cend ());
  }

  template <typename TypeP>
  TypeP
  extra_data () const
  {
    return boost::any_cast<TypeP> (m_extra_data);
  }

  template <typename ExceptionP>
  void
  nest_exception (ExceptionP ex)
  {
    m_nested.push_back (std::make_exception_ptr (std::move (ex)));
  }

  void
  add_context (std::string context)
  {
    m_contexts.push_back (std::move (context));
  }

  template <typename TypeP>
  void
  set_extra_data (TypeP extra)
  {
    m_extra_data = extra;
  }

private:
  std::vector<std::exception_ptr> m_nested;
  std::vector<std::string> m_contexts;
  boost::any m_extra_data;
};

class BasicStdExceptionImplementation : public std::exception
{
public:
  explicit BasicStdExceptionImplementation (std::string const& w)
    : reason {w}
  {}

  explicit BasicStdExceptionImplementation (char const* w)
    : reason {w}
  {}

  char const* what() const noexcept override
  {
    return reason.c_str ();
  }

private:
  std::string reason;
};

template <typename StdExceptionP>
class BasicException : public Exception, public StdExceptionP
{
public:
  using StdExceptionP::StdExceptionP;
};

using RuntimeError = BasicException<std::runtime_error>;
using LogicError = BasicException<std::logic_error>;

class Bug : BasicException<BasicStdExceptionImplementation>
{
public:
  using BasicException<BasicStdExceptionImplementation>::BasicException;
};

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_EXCEPTIONS_HH
