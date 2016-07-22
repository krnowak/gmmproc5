#ifndef GMMPROC_XML_STRUCTURED_NODE_HH
#define GMMPROC_XML_STRUCTURED_NODE_HH

#include <gmmproc/xml/structured/detail/getters.hh>
#include <gmmproc/xml/structured/child.hh>
#include <gmmproc/xml/structured/attribute.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

template <typename... ChildrenP>
class Node : private virtual Detail::Storage<ChildrenP...>,
             public Detail::Getters<ChildrenP...>::Type
{
  Node (Xml::Node& node)
    : Detail::Storage<NodeSchemaP> {Detail::Initializer<ChildrenP...>::get_tuple {node}}
  {}
  ~Node () = default;

  Node (Node const&) = delete;
  Node (Node&&) = default;

  Node& operator= (Node const&) = delete;
  Node& operator= (Node&&) = default;
};

template <typename NodeTagP>
using DocNode = Node<Basic::Child<Basic::Single, NodeTagP>>;

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_NODE_HH
