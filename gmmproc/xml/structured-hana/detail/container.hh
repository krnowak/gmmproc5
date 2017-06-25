#ifndef GMMPROC_XML_STRUCTURED_DETAIL_CONTAINER_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_CONTAINER_HH

#include <gmmproc/xml/structured/detail/getters.hh>
#include <gmmproc/xml/structured/detail/storage.hh>
#include <gmmproc/xml/structured/detail/utils.hh>

#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename StorageP,
          typename GettersSubclassP>
class ContainerHelper
{
public:
  using Storage = StorageP;
  using Getters = GettersSubclassP;
};

// explicitly specified deduction guide
template <typename StorageP,
          typename GettersSubclassP>
ContainerHelper(StorageP, GettersSubclassP) -> ContainerHelper<StorageP, GettersSubclassP>;

template <typename PartsP>
constexpr auto
get_container_helper (PartsP parts)
{
  auto getters_info {NoADL::resolve_getters_info (parts)};
  auto getters_subclass_type {NoADL::get_getters_subclass_type (getters_info)};

  return ContainerHelper {getters_info.container_info.storage_type,
                          getters_subclass_type};
}

template <typename PartsP>
using ContainerHelperT = typename decltype(NoADL::get_container_helper (std::declval<PartsP> ()))::type;
template <typename PartsP>
using ContainerStorageT = typename ContainerHelperT<PartsP>::Storage;
template <typename PartsP>
using ContainerGettersT = typename ContainerHelperT<PartsP>::Getters;


} // Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_CONTAINER_HH
