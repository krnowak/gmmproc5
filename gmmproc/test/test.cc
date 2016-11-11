#include <iostream>

namespace test_fallthrough_nodiscard_maybe_unused_attributes
{

int f1()
{
  return 42;
}

[[nodiscard]] int f2()
{
  [[maybe_unused]] auto unused = f1();

  switch (f1())
  {
  case 17:
    f1();
    [[fallthrough]];
  case 42:
    f1();
  }
  return f1();
}

}

int
main ()
{
  std::cout << __cplusplus << "\n";
#ifdef __GNUC__
  std::cout << "__GNUC__\n";
#endif
#ifdef __clang__
  std::cout << "__clang__\n";
#endif
}
