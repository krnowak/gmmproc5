#ifndef GMMPROC_XML_STRUCTURED_DETAIL_WRAPPERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_WRAPPERS_HH

namespace Gmmproc::Xml::Structured::Detail::W
{

template <typename AnythingP>
using TypeWrapper = hana::type<AnythingP>;

template <typename AnythingP>
constexpr auto type_wrap (AnythingP)
{
  return TypeWrapper<AnythingP> {};
}

template <typename AnythingP>
constexpr auto type_unwrap (TypeWrapper<AnythingP>)
{
  return TypeWrapper<AnythingP> {};
}

template <typename ObjectP>
class ObjectWrapper
{
public:
  ObjectP object;
};

// explicitly specified deduction guide
template <typename ObjectP>
ObjectWrapper(ObjectP) -> ObjectWrapper<ObjectP>;

template <typename ObjectP>
decltype(auto) object_wrap (ObjectP&& object)
{
  return ObjectWrapper {std::forward<ObjectP> (object)};
}

template <typename ObjectP>
decltype(auto) object_unwrap (ObjectWrapper<ObjectP> wrapper)
{
  return std::forward<ObjectP> (wrapper.object);
}

} // namespace Gmmproc::Xml::Structured::Detail::W

#endif // GMMPROC_XML_STRUCTURED_DETAIL_WRAPPERS_HH
