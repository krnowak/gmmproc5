#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/counting_range.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <iostream>
#include <unordered_map>
#include <memory>

class C
{
public:
  C (int i)
    : a {i},
      b {(i + 1) * 2}
  {}

  int a = 0;
  int b = 0;
};

using Map = std::unordered_map<int, C>;

class BFromC
{
public:
  using result_type = int&;

  int&
  operator() (Map::value_type& v) const
  {
    return v.second.b;
  }
};

class Base
{
public:
  virtual ~Base();
  virtual std::unique_ptr<Base> foo () = 0;
};

class Derived : public Base
{
  virtual std::unique_ptr<Base> foo () override
  {
    return std::unique_ptr<Base> {std::make_unique<Derived> ()};
  }
};

int
main ()
{
  Map m;
  for (auto i : boost::counting_range(0, 10))
  {
    m.emplace (i, i);
  }

  BFromC w;
  for (auto& i : boost::make_iterator_range(boost::make_transform_iterator (m.begin (), w),
                                            boost::make_transform_iterator (m.end (), w)))
  {
    std::cout << i << std::endl;
  }
}
