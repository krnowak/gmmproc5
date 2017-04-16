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

template <typename PartsP,
          typename ExtraDataImplTypeP>
constexpr auto
get_extended_parts (PartsP parts,
                    ExtraDataImplTypeP extra_data_impl_type)
{
  if constexpr (extra_data_type == hana::type_c<void>)
  {
    return parts;
  }
  else
  {
    return hana::append (parts,
                         ExtraData<ExtraDataImplTypeP> {});
  }
}

template <typename PartsP,
          typename ExtraDataImplTypeP>
constexpr auto
get_container_helper (PartsP parts,
                      ExtraDataImplTypeP extra_data_impl_type)
{
  auto extended_parts {NoADL::get_extended_parts (parts, extra_data_impl_type)};
  auto resolved_storage_tag {NoADL::resolve_storage_tag (storage_tag,
                                                         extended_parts)};
  auto getters_info {NoADL::resolve_getters_info (resolved_storage_tag)};

  auto storage_type {resolved_storage_tag.storage_hana_type};
  auto getters_subclass_type {NoADL::get_getters_subclass_type (getters_info)};

  return ContainerHelper<StorageP, GettersSubclassP> {storage_type,
                                                      getters_subclass_type};
}

template <typename PartsP,
          typename ExtraDataImplTypeP>
using ContainerHelperT = typename decltype(NoADL::get_container (std::declval<PartsP> (),
                                                                 std::declval<ExtraDataImplTypeP> ()))::type;
template <typename PartsP,
          typename ExtraDataImplTypeP = hana::type<void>>
using ContainerStorageT = typename ContainerHelperT<PartsP, ExtraDataImplTypeP>::Storage;

template <typename PartsP,
          typename ExtraDataImplTypeP = hana::type<void>>
using ContainerGettersT = typename ContainerHelperT<PartsP, ExtraDataImplTypeP>::Getters;


} // Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_CONTAINER_HH
