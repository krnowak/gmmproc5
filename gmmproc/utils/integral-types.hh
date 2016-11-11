#ifndef GMMPROC_UTILS_INTEGRAL_TYPES_HH
#define GMMPROC_UTILS_INTEGRAL_TYPES_HH

#include <type_traits>

namespace Gmmproc
{

namespace Utils
{

template <std::size_t ValueP>
using StdSizeTConstant = std::integral_constant<std::size_t, ValueP>;

template <bool ValueP>
using BoolConstant = std::integral_constant<bool, ValueP>;

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_INTEGRAL_TYPES_HH
