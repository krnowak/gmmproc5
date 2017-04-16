#ifndef GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH

#include <gmmproc/xml/structured/detail/container.hh>

#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename XmlViewTypeP,
          typename... PartP>
class Node : private virtual Detail::ContainerStorageT<hana::tuple<PartP...>>,
             public Detail::ContainerGettersT<hana::tuple<PartP...>>
{
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
