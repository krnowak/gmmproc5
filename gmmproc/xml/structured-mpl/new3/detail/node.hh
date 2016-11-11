#ifndef GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH

#include <gmmproc/xml/structured/detail/getters.hh>
#include <gmmproc/xml/structured/detail/storage.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename XmlViewTypeP, typename... PartP>
class Node : private virtual Storage<PartP...>,
             public Getters<PartP...>::Type
{
public:
  Node (XmlViewTypeP& xml_view)
    : Storage<PartP...> {Mpl::ApplyT<typename PartP::GetGenerator>::get(xml_view->as_basic_node ())...}
  {}
  ~Node () = default;

  Node (Node const&) = delete;
  Node (Node&&) = default;

  Node& operator= (Node const&) = delete;
  Node& operator= (Node&&) = default;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_NODE_HH
