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

template <typename AttributeKeyListP, typename ChildListP, typename HasTextP = std::false_type>
class NodeSchema
{
public:
  using AttributeKeyList = AttributeKeyListP;
  using ChildList = ChildListP;
  using HasText = HasTextP;
};

template <typename NodeSchemaP>
class Node : private virtual Detail::Storage<NodeSchemaP>,
             public Detail::Getters<NodeSchemaP>::Type
{
public:
  Node (Xml::Node& node)
    : Detail::Storage<NodeSchemaP> {Detail::Initializer<NodeSchemaP>::get_tuple {node}}
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
