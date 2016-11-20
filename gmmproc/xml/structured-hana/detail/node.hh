#ifndef GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH

#include <gmmproc/xml/structured/detail/utils.hh>
#include <gmmproc/xml/structured/detail/getters.hh>
#include <gmmproc/xml/structured/detail/storage.hh>

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename StorageTypeP, typename GettersSubclassP>
class NodeDetails : private virtual StorageTypeP,
                    public GettersSubclassP
{};

template <typename StorageHanaTypeP, typename GettersSubclassHanaTypeP>
constexpr auto
get_node_details_hana_type (StorageHanaTypeP, GettersSubclassHanaTypeP)
{
  return hana::type_c<
    NodeDetails<
      typename StorageHanaTypeP::type,
      typename GettersSubclassHanaTypeP::type
    >
  >;
}

template <typename StorageTagP, typename PartsP>
constexpr auto
get_node_details (StorageTagP storage_tag, PartsP parts)
{
  auto resolved_storage_tag {NoADL::resolve_storage_tag (storage_tag, parts)};
  auto getters_info {NoADL::resolve_getters_info (resolved_storage_tag)};

  auto storage_hana_type {resolved_storage_tag.storage_hana_type};
  auto getters_subclass_hana_type {NoADL::get_getters_subclass_type (getters_info)};

  return NoADL::get_node_details_hana_type (storage_hana_type,
                                            getters_subclass_hana_type);
}

template <typename StorageTagP, typename PartsP>
using NodeDetailsT = typename decltype(NoADL::get_node_details (std::declval<StorageTagP> (),
                                                                std::declval<PartsP> ()))::type;

template <typename XmlViewTypeP, typename StorageTagP, typename... PartP>
class Node : public NodeDetailsT<StorageTagP,
                                 hana::tuple<hana::type<PartP>...>>
{
private:
  // TODO: get storage container type here
  using StorageContainer = ;

public:
  Node () = default;
  Node (XmlViewTypeP& /* xml_view */) {}
  #if 0
  Node (XmlViewType& xml_view)
    : StorageContainer {NoADL::member_from_node ()...}
  {}

    {xml_view /*Mpl::ApplyT<typename PartP::GetGenerator>::get(xml_view->as_basic_node ())...*/}
  {}
  #endif
  ~Node () = default;

  Node (Node const&) = delete;
  Node (Node&&) = default;

  Node& operator= (Node const&) = delete;
  Node& operator= (Node&&) = default;
};

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH
