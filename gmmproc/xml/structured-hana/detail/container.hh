#ifndef GMMPROC_XML_STRUCTURED_DETAIL_CONTAINER_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_CONTAINER_HH

#include <gmmproc/xml/structured/detail/getters.hh>
#include <gmmproc/xml/structured/detail/storage.hh>
#include <gmmproc/xml/structured/detail/utils.hh>

#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename StorageTypeP, typename GettersSubclassP>
class Container : private virtual StorageTypeP,
                  public GettersSubclassP
{};

template <typename StorageHanaTypeP, typename GettersSubclassHanaTypeP>
constexpr auto
get_container_hana_type (StorageHanaTypeP,
                         GettersSubclassHanaTypeP)
{
  return hana::type_c<
    Container<
      typename StorageHanaTypeP::type,
      typename GettersSubclassHanaTypeP::type
    >
  >;
}

template <typename StorageTagP, typename PartsP>
constexpr auto
get_container (StorageTagP storage_tag,
               PartsP parts)
{
  auto resolved_storage_tag {NoADL::resolve_storage_tag (storage_tag, parts)};
  auto getters_info {NoADL::resolve_getters_info (resolved_storage_tag)};

  auto storage_hana_type {resolved_storage_tag.storage_hana_type};
  auto getters_subclass_hana_type {NoADL::get_getters_subclass_type (getters_info)};

  return NoADL::get_container_hana_type (storage_hana_type,
                                         getters_subclass_hana_type);
}

template <typename StorageTagP, typename PartsP>
using ContainerT = typename decltype(NoADL::get_container (std::declval<StorageTagP> (),
                                                           std::declval<PartsP> ()))::type;

} // Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_CONTAINER_HH
