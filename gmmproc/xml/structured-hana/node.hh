#ifndef GMMPROC_XML_STRUCTURED_NODE_HH
#define GMMPROC_XML_STRUCTURED_NODE_HH

#include <gmmproc/xml/structured/detail/node.hh>
#include <gmmproc/xml/structured/hana-storage.hh>
#include <gmmproc/xml/structured/child.hh>

#include <gmmproc/xml/xml.hh>

namespace Gmmproc::Xml::Structured
{

template <typename StorageTagP,
          typename... PartP>
using CustomNode = Detail::Node<NodeView,
                                StorageTagP,
                                PartP...>;

template <typename StorageTagP,
          typename PartP>
using CustomDocument = Detail::Node<DocumentView,
                                    StorageTagP,
                                    PartP>;

template <typename... PartP>
using Node = CustomNode<Hana::StorageTag,
                        PartP...>;

template <typename AccessKeyP,
          typename NodeTagP>
using Document = CustomDocument<Hana::StorageTag,
                                Basic::Child<AccessKeyP,
                                             Basic::Single,
                                             NodeTagP>>;

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_NODE_HH
