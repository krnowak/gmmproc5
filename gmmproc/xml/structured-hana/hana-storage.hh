#ifndef GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Hana
{

class StorageTag
{};

template <typename... TypeP>
class Storage
{
  template <typename IndexP>
  constexpr decltype(auto)
  operator[] (IndexP index)
  {
    return storage[index];
  }

private:
  boost::hana::tuple<TypeP...> storage;
};

template <typename... HanaTypeP>
constexpr auto
get_storage_type (StorageTag, HanaTypeP...)
{
  return boost::hana::type_c<Storage<typename HanaTypeP::type...>>;
}

} // namespace Gmmproc::Xml::Structured::Hana

#endif // GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH
