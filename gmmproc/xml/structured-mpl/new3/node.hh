#ifndef GMMPROC_XML_STRUCTURED_NODE_HH
#define GMMPROC_XML_STRUCTURED_NODE_HH

#include <gmmproc/xml/structured/detail/node.hh>
#include <gmmproc/xml/structured/child.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

template <typename... PartP>
using Node = Detail::Node<NodeView, PartP...>;

template <typename NodeTagP>
using Document = Detail::Node<DocumentView, Basic::Child<Basic::Single, NodeTagP>>;

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_NODE_HH
