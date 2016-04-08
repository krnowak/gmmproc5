#ifndef GMMPROC_UTILS_TYPE_TO_TYPE_HH
#define GMMPROC_UTILS_TYPE_TO_TYPE_HH

#include <type_traits>

namespace Gmmproc
{

namespace Utils
{

template <typename Type>
class TypeToType;

template <typename Type>
using TypeToTypeT = typename TypeToType<Type>::Type;

/**
 * TypeToTypeBase is a small helper for registering new TypeToType
 * specializations.
 *
 * An example specialization could be:
 * class Tag;
 * class Target {...};
 *
 * namespace Gmmproc { namespace Utils {
 *
 * template <>
 * class TypeToType<Tag> : public TypeToTypeBase<Target>
 * {};
 */
template <typename To>
class TypeToTypeBase
{
public:
  using Type = To;
};

template <typename Type, typename = void>
class HasToType : public std::false_type
{};

template <typename Type>
class HasToType<Type, TypeToTypeT<Type>> : public std::true_type
{};

} // namespace Utils

} // namespace Gmmproc

#endif // GMMPROC_UTILS_TYPE_TO_TYPE_HH
