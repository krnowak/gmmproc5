#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/range/iterator_range_core.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <iostream>
#include <type_traits>
#include <vector>
#include <functional>

struct Int
{
  Int(int i, int mul) : m_i {i * mul} {}

  int m_i;
};

// stream output
std::ostream&
operator<<(std::ostream& os,
           Int const& i)
{
  return (os << "STRUCT(" << i.m_i << ")");
}

using IntV = std::vector<int>;
using SIter = boost::transform_iterator<std::function<Int(int)>, IntV::iterator>;

int
main ()
{
  IntV v{1, 2, 3, 4, 5};
  auto mul = 3;
  auto convert = [mul](int i) { return Int {i, mul}; };
  auto b = SIter (v.begin (), convert);
  auto e = SIter (v.end (), convert);
  auto range = boost::make_iterator_range (b, e);

  for (auto const& i : range)
  {
    std::cout << i << std::endl;
  }
}
