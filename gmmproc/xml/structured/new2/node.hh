#ifndef GMMPROC_XML_STRUCTURED_NODE_HH
#define GMMPROC_XML_STRUCTURED_NODE_HH

#include <gmmproc/xml/structured/detail/getters.hh>
#include <gmmproc/xml/structured/child.hh>
#include <gmmproc/xml/structured/attribute.hh>

#include <boost/mpl/vector.hpp>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

template <typename... PartP>
class Node : private virtual Detail::Storage<PartP...>,
             public Detail::Getters<PartP...>::Type
{
public:
  Node (Xml::Node& node)
    : Detail::Storage<PartP...> {Mpl::ApplyT<typename PartP::GetGenerator>::get(node)...}
  {}
  ~Node () = default;

  Node (Node const&) = delete;
  Node (Node&&) = default;

  Node& operator= (Node const&) = delete;
  Node& operator= (Node&&) = default;
};

template <typename NodeTagP>
using DocNode = Node<
  NodeSchema<
    boost::mpl::vector<>, // no attributes
    boost::mpl::vector<
      Basic::Child<Basic::Single, NodeTagP>
    >
  >
>;

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_NODE_HH
