#ifndef GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Hana
{

class StorageTag
{};

template <typename... TypeP>
constexpr auto
get_storage_type (StorageTag,
                  TypeP...)
{
  return boost::hana::type_c<boost::hana::tuple<typename TypeP::type...>>;
}

} // namespace Gmmproc::Xml::Structured::Hana

#endif // GMMPROC_XML_STRUCTURED_HANA_STORAGE_HH
