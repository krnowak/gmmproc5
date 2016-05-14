#include <boost/mpl/inherit.hpp>

#include <iostream>

class B
{
public:
  B(int i)
    : m{i}
  {
    std::cout << "B::B(int): " << m << "\n";
  }

  int m;
};

class C1 : public virtual B
{
public:
  C1 (int& i)
    : B{i + 1}
  {
    i += 1;
  }
};

class C2 : public virtual B
{
public:
  C2 (int& i)
    : B{i * 2}
  {
    i *= 2;
  }
};

class C3 : public virtual B
{
public:
  C3 (int& i)
    : B{i * i}
  {
    i *= i;
  }
};

template <typename... C>
class A : public C...
{
public:
  A(int &i)
    : B{i}, C{i}...
  {}
};

using A123 = A<C1, C2, C3>;

int
main ()
{
  auto i = 5;

  A123{i};
  std::cout << i << "\n";
}
