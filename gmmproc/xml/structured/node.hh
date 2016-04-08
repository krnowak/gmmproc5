#ifndef GMMPROC_XML_STRUCTURED_NODE_HH
#define GMMPROC_XML_STRUCTURED_NODE_HH

#include <gmmproc/xml/structured/detail/processor-and-getters.hh>
#include <gmmproc/xml/structured/child.hh>
#include <gmmproc/xml/structured/attribute.hh>

#include <boost/mpl/vector.hpp>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Registry
{

template <typename AttributeP>
class AttrInfo;

template <typename NodeP>
class NodeInfo;

} // namespace Registry

template <typename AttrListP, typename ChildListP, typename HasTextTypeP>
class Node : public typename ProcessorAndGetters<AttrListP, ChildListP, HasTextTypeP>::Type
{
  // TODO
};

template <typename ChildTagP>
using DocNode = Node<
  mpl::vector<
    // no attributes
  >,
  mpl::vector<
    Basic::Child<Basic::Single, ChildTagP>
  >
>;

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_NODE_HH
