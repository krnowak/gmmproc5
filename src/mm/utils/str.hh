#ifndef MM_UTILS_STR_HH
#define MM_UTILS_STR_HH

#include <array>
#include <type_traits>

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

template <typename Type>
class IsStaticString : public std::false_type
{};

template <char... Chars>
class IsStaticString<StaticString<Chars...>> : public std::true_type
{};

} // namespace Utils

} // namespace Mm

#endif // MM_UTILS_STR_HH
