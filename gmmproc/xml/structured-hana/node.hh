#ifndef GMMPROC_XML_STRUCTURED_NODE_HH
#define GMMPROC_XML_STRUCTURED_NODE_HH

#include <gmmproc/xml/structured/detail/node.hh>
#include <gmmproc/xml/structured/child.hh>

#include <gmmproc/xml/xml.hh>

namespace Gmmproc::Xml::Structured
{

template <typename... PartP>
using CustomNode = Detail::Node<NodeView,
                                PartP...>;

template <typename PartP>
using CustomDocument = Detail::Node<DocumentView,
                                    PartP>;

template <typename... PartP>
using Node = CustomNode<PartP...>;

template <typename AccessKeyP,
          typename NodeTagP>
using Document = CustomDocument<Basic::Child<AccessKeyP,
                                             Basic::Single,
                                             NodeTagP>>;

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_NODE_HH
