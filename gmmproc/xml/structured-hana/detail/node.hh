#ifndef GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH

#include <gmmproc/xml/structured/detail/getters.hh>
#include <gmmproc/xml/structured/detail/storage.hh>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename ResolvedStorageTagP>
using NodeStorageTypeT = typename ResolvedStorageTagP::StorageType::type;

template <typename XmlViewTypeP, typename ResolvedStorageTagP>
class Node : private virtual NodeStorageTypeT<ResolvedStorageTagP>,
             public GettersTypeT<ResolvedStorageTagP>
{
public:
  Node (XmlViewTypeP& xml_view)
    : NodeStorageTypeT<ResolvedStorageTagP> {xml_view /*Mpl::ApplyT<typename PartP::GetGenerator>::get(xml_view->as_basic_node ())...*/}
  {}
  ~Node () = default;

  Node (Node const&) = delete;
  Node (Node&&) = default;

  Node& operator= (Node const&) = delete;
  Node& operator= (Node&&) = default;
};

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH
