#ifndef MM_UTILS_STR_HH
#define MM_UTILS_STR_HH

#include <array>

namespace Mm
{

namespace Utils
{

template <char... Chars>
class StaticString
{
public:
  using Type = std::array<char, sizeof... (Chars) + 1>;

  static constexpr Type raw {Chars..., '\0'};
};

template <char... Chars>
constexpr typename StaticString<Chars...>::Type StaticString<Chars...>::raw;

} // namespace Utils

} // namespace Mm

#endif // MM_UTILS_STR_HH
