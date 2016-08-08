#ifndef GMMPROC_XML_XML_PUGI_HH
#define GMMPROC_XML_XML_PUGI_HH

#ifndef GMMPROC_XML_INCLUDING_IMPL

#error "This file should not be included directly"

#endif

#include "xml.hh"

#include <pugixml.hpp>

#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <experimental/optional>

#include <memory>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>

namespace Gmmproc
{

namespace Xml
{

namespace PugiXmlDetails
{

// noexcept swap pugi wrappers

// swappable xml node

class SwappableXmlNode : public pugi::xml_node
{
public:
  using pugi::xml_node::xml_node;

  SwappableXmlNode (pugi::xml_node const& n);

  void swap (SwappableXmlNode& node) noexcept;
};

// swappable xml attribute

class SwappableXmlAttribute : public pugi::xml_node
{
public:
  using pugi::xml_node::xml_node;

  SwappableXmlAttribute (pugi::xml_node const& n);

  void swap (SwappableXmlAttribute& node) noexcept;
};

// impls

// basic node

class BasicNodeImpl
{
public:
  BasicNodeImpl (pugi::xml_document* d);
  BasicNodeImpl (pugi::xml_document* d,
                 SwappableXmlNode n);
  ~BasicNodeImpl () noexcept;

  BasicNodeImpl (BasicNodeImpl const& other);
  BasicNodeImpl (BasicNodeImpl&& other) noexcept;

  void swap (BasicNodeImpl& other) noexcept;

  pugi::xml_node get_node () const;

  pugi::xml_document* doc_ptr;
  Type::Variant<xml_document*, SwappableXmlNode> doc_or_node;
};

// attribute

class AttributeImpl
{
public:
  AttributeImpl (pugi::xml_document* d,
                 SwappableXmlNode p,
                 SwappableXmlAttribute a);
  ~AttributeImpl () noexcept;

  AttributeImpl (AttributeImpl const& other);
  AttributeImpl (AttributeImpl&& other) noexcept;

  void swap (AttributeImpl& other) noexcept;

  pugi::xml_document* doc_ptr;
  SwappableXmlNode parent;
  SwappableXmlAttribute attribute;
};

// text

class TextImpl
{
public:
  TextImpl (pugi::xml_document* d,
            SwappableXmlNode n);
  ~TextImpl () noexcept;

  TextImpl (TextImpl const& other);
  TextImpl (TextImpl&& other) noexcept;

  void swap (TextImpl &other) noexcept;

  pugi::xml_document* doc_ptr;
  SwappableXmlNode node;
};

// node

class NodeImpl
{
public:
  NodeImpl (pugi::xml_document* d,
            SwappableXmlNode n);
  ~NodeImpl () noexcept;

  NodeImpl (NodeImpl const& other);
  NodeImpl (NodeImpl&& other) noexcept;

  void swap (NodeImpl &other) noexcept;

  pugi::xml_document* doc_ptr;
  SwappableXmlNode node;
};

// document

class DocumentImpl
{
public:
  DocumentImpl (pugi::xml_document* d);
  ~DocumentImpl () noexcept;

  DocumentImpl (DocumentImpl const& other);
  DocumentImpl (DocumentImpl&& other) noexcept;

  void swap (DocumentImpl &other) noexcept;

  pugi::xml_document* doc_ptr;
};

// bundle

class BundleImpl
{
public:
  BundleImpl ();
  ~BundleImpl () noexcept;

  BundleImpl (BundleImpl const& other) = delete;
  BundleImpl (BundleImpl&& other) noexcept;

  void swap (BundleImpl &other) noexcept;

  std::unique_ptr<xml_document> doc;
};

// transformers and predicates

// node

template <Helpers::WrapperType TypeV>
class NodeTransform
{
public:
  NodeTransform ();
  NodeTransform (pugi::xml_document* d);

  typename Helpers::Creator<TypeV>::NodeType operator () (pugi::xml_node const& n) const;

private:
  pugi::xml_document* doc;
};

class NodePredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

// attribute

template <Helpers::WrapperType TypeV>
class AttributeTransform
{
public:
  AttributeTransform ();
  AttributeTransform (pugi::xml_document* d,
                      pugi::xml_node const& p);

  typename Helpers::Creator<TypeV>::AttributeType operator () (pugi::xml_attribute const& a) const;

private:
  pugi::xml_document* doc_ptr;
  pugi::xml_node parent;
};

// text

template <Helpers::WrapperType TypeV>
class TextTransform
{
public:
  TextTransform ();
  TextTransform (pugi::xml_document* d);

  typename Helpers::Creator<TypeV>::TextType operator () (pugi::xml_node const& n) const;

private:
  pugi::xml_document* doc;
};

class TextPredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

// node or text

template <Helpers::WrapperType TypeV>
class NodeOrTextTransform
{
public:
  NodeOrTextTransform ();
  NodeOrTextTransform (pugi::xml_document* d);

  typename Helpers::Creator<TypeV>::NodeOrTextType operator () (pugi::xml_node const& n) const;

private:
  pugi::xml_document* doc;
};

class NodeOrTextPredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

template <typename TransformP, typename PredicateP>
using IteratorType = boost::transform_iterator<TransformP, boost::filter_iterator<PredicateP, pugi::xml_node_iterator>>;

template <typename WalkerTypeP>
class Walker;

template <typename WalkerTypeP>
class NodeOrDocVisitor;

template <typename WalkerTypeP>
class WalkerHost;

template <typename HostTypeP>
class PugiWalker : public pugi::xml_tree_walker
{
public:
  PugiWalker (WalkerHost<HostTypeP> *h,
              pugi::xml_document* d)
    : host {h},
      doc {d}
  {}

private:
  virtual bool
  for_each (pugi::xml_node& node) override;

  virtual bool
  end (pugi::xml_node& node) override;

  bool
  run_postprocessing ();

  WalkerHost<HostTypeP>* host;
  pugi::xml_document* doc;
  std::stack<std::pair<Node, int>> nodes;
};

bool
PugiWalker::for_each (pugi::xml_node& node)
{
  auto const d = depth ();

  switch (node.type ())
  {
  case pugi::node_pcdata:
  case pugi::node_cdata:
    {
      auto t = Text::create_const
      Text t {node};

      return host->text (t, d);
    }

  case pugi::node_element:
    {
      if (!run_postprocessing ())
      {
        return false;
      }

      Node n {NodeWrapper {std::ref (*doc), node}};
      nodes.push (std::make_pair (n, d));
      return walker.node (n, d);
    }

  default:
    return true;
  }
}

bool
PugiWalker::end (pugi::xml_node&)
{
  auto result = run_postprocessing ();
  doc = nullptr;
  return result;
}

bool
PugiWalker::run_postprocessing ()
{
  auto const d = depth ();

  while (!nodes.empty ())
  {
    auto p = nodes.top ();

    if (p.second < d)
    {
      break;
    }
    nodes.pop ();
    if (!walker.postprocess_node (p.first, p.second))
    {
      return false;
    }
  }
  return true;
}

template <typename HostTypeP>
class NodeOrDocVisitor : public boost::static_visitor<pugi::xml_node>
{
public:
  NodeOrDocVisitor ();

  pugi::xml_node
  operator() (typename HostTypeP::Node const& node)
  {
    return WalkerHost<HostTypeP>::get_node (node.as_basic_node ());
  }

  pugi::xml_node
  operator() (typename HostTypeP::Document const& doc)
  {
    return WalkerHost<HostTypeP>::get_node (doc.as_basic_node ());
  }
};

template <typename HostTypeP>
class NodeOrDocForDocVisitor : public boost::static_visitor<pugi::xml_document*>
{
public:
  NodeOrDocVisitor ();

  pugi::xml_document*
  operator() (typename HostTypeP::Node const& node)
  {
    return WalkerHost<HostTypeP>::get_doc (node.as_basic_node ());
  }

  pugi::xml_document*
  operator() (typename HostTypeP::Document const& doc)
  {
    return WalkerHost<HostTypeP>::get_doc (doc.as_basic_node ());
  }
};

class ConstHost;
const MutHost;

class BasicNodeImplNodeVisitor : public boost::static_visitor<pugi::xml_node>
{
public:
  pugi::xml_node
  operator() (SwappableXmlNode const& node)
  {
    return node;
  }

  pugi::xml_node
  operator() (pugi::xml_document const* doc_ptr)
  {
    return doc_ptr->root ();
  }
};

template <typename HostTypeP, typename RealWalkerP>
class WalkerHost
{
public:
  WalkerHost (RealWalkerP *w, typename HostType::NodeOrDoc node_or_doc)
    : walker {w},
      doc {get_doc_from_variant (node_or_doc)}
  {}

  PugiWalker<HostTypeP> pugi_walker() const
  {
    return Walker<HostTypeP> {this, doc};
  }

  NodeOrDocVisitor<WalkerTypeP> visitor() const
  {
    return NodeOrDocVisitor<HostTypeP> {};
  }

private:
  friend class PugiWalker<HostTypeP>;
  friend class NodeOrDocVisitor<WalkerTypeP>;

  static pugi::xml_document* get_doc_from_variant (typename HostType::NodeOrDoc node_or_doc)
  {
    return boost::apply_visitor (NodeOrDocForDocVisitor{}, node_or_doc);
  }

  static pugi::xml_document* get_doc (typename HostTypeP::BasicNode const& basic_node)
  {
    return basic_node->impl.doc_ptr;
  }

  static pugi::xml_node get_node (typename HostTypeP::BasicNode const& basic_node)
  {
    return boost::apply_visitor (BasicNodeImplNodeVisitor {}, basic_node->impl.doc_or_node);
  }

  bool doc (typename WalkerTypeP::Document& doc, int depth) const
  {
    return walker->doc (doc, depth);
  }

  bool node (typename WalkerTypeP::Node& node, int depth) const
  {
    return walker->node (node, depth);
  }

  bool text (typename WalkerTypeP::Text& text, int depth) const
  {
    return walker->text (node, depth);
  }

  bool postprocess_node (typename WalkerTypeP::Node& node, int depth) const
  {
    return walker->postprocess_node (node, depth);
  }

  bool postprocess_doc (typename WalkerTypeP::Document& doc, int depth) const
  {
    return walker->postprocess_doc (doc, depth);
  }

  RealWalker *walker;
  pugi::xml_document *doc;
};

using MutWalkerHost = WalkerHost<MutHost>;
using ConstWalkerHost = WalkerHost<ConstHost>;

} // namespace PugiXmlDetails

class XmlImpl
{
public:
  using AttributeImpl = PugiXmlDetails::AttributeImpl;
  using TextImpl = PugiXmlDetails::TextImpl;
  using NodeImpl = PugiXmlDetails::NodeImpl;
  using DocumentImpl = PugiXmlDetails::DocumentImpl;
  using BundleImpl = PugiXmlDetails::BundleImpl;

  using NodeRange = boost::iterator_range<PugiXmlDetails::IteratorType<NodeTransform<Helpers::WrapperType::Mutable>, NodePredicate>>;
  using NodeConstRange = boost::iterator_range<PugiXmlDetails::IteratorType<NodeTransform<Helpers::WrapperType::Const>, NodePredicate>>;
  using AttributeRange = boost::iterator_range<boost::transform_iterator<AttributeTransform<Helpers::WrapperType::Mutable>, pugi::xml_attribute_iterator>>;
  using AttributeConstRange = boost::iterator_range<boost::transform_iterator<AttributeTransform<Helpers::WrapperType::Const>, pugi::xml_attribute_iterator>>;
  using TextRange = boost::iterator_range<PugiXmlDetails::IteratorType<TextTransform<Helpers::WrapperType::Mutable>, TextPredicate>>;
  using TextConstRange = boost::iterator_range<PugiXmlDetails::IteratorType<TextTransform<Helpers::WrapperType::Const>, TextPredicate>>;
  using NodeOrTextRange = boost::iterator_range<PugiXmlDetails::IteratorType<NodeOrTextTransform<Helpers::WrapperType::Mutable>, NodeOrTextPredicate>>;
  using NodeOrTextConstRange = boost::iterator_range<PugiXmlDetails::IteratorType<NodeOrTextTransform<Helpers::WrapperType::Const>, NodeOrTextPredicate>>;

  using MutWalkerTypeImpl = PugiXmlDetails::MutWalkerHost;
  using ConstWalkerTypeImpl = PugiXmlDetails::ConstWalkerHost;
};

namespace PugiXmlDetails
{

// swappable xml node

inline
SwappableXmlNode::SwappableXmlNode (pugi::xml_node const& n)
  : pugi::xml_node {n.internal_object ()}
{}

inline void
SwappableXmlNode::swap (SwappableXmlNode& node) noexcept
{
  using std::swap;

  swap (_root, node._root);
}

// swappable xml attribute

inline
SwappableXmlNode::SwappableXmlNode (pugi::xml_attribute const& a)
  : pugi::xml_attribute {a.internal_object ()}
{}

inline void
SwappableXmlNode::swap (SwappableXmlNode& node) noexcept
{
  using std::swap;

  swap (_attr, node._attr);
}

// basic node impl

inline
BasicNodeImpl::BasicNodeImpl (pugi::xml_document* d)
  : doc_ptr {d}
    doc_or_node {d}
{}

inline
BasicNodeImpl::BasicNodeImpl (pugi::xml_document* d,
                              SwappableXmlNode n)
  : doc_ptr {d},
    doc_or_node {std::move (n)}
{}

inline
BasicNodeImpl::~BasicNodeImpl () noexcept = default;

inline
BasicNodeImpl::BasicNodeImpl (BasicNodeImpl const& other)
  : doc_ptr {other.doc_ptr},
    doc_or_node {other.doc_or_node}
{}

inline
BasicNodeImpl::BasicNodeImpl (BasicNodeImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)},
    doc_or_node {std::move (other.doc_or_node)}
{}

inline void
BasicNodeImpl::swap (BasicNodeImpl& other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
  swap (doc_or_node, other.doc_or_node);
}

inline pugi::xml_node
BasicNodeImpl::get_node () const
{
  return boost::apply_visitor (BasicNodeImplNodeVisitor {}, doc_or_node);
}

// attribute impl

inline
AttributeImpl::AttributeImpl (pugi::xml_document* d,
                              SwappableXmlNode p,
                              SwappableXmlAttribute a)
  : doc_ptr {d},
    parent {std::move (p)},
    attribute {std::move (a)}
{}

inline
AttributeImpl::~AttributeImpl () noexcept = default;

inline
AttributeImpl::AttributeImpl (AttributeImpl const& other)
  : doc_ptr {other.doc_ptr},
    parent {other.parent},
    attribute {other.attribute}
{}

AttributeImpl::AttributeImpl (AttributeImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)},
    parent {std::move (other.parent)},
    attribute {std::move (other.attribute)}
{}

void AttributeImpl::swap (AttributeImpl& other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
  swap (parent, other.parent);
  swap (attribute, other.attribute);
}

// text impl

TextImpl::TextImpl (pugi::xml_document* d,
                    SwappableXmlNode n)
  : doc_ptr {d},
    node {std::move (n)}
{}

TextImpl::~TextImpl () noexcept = default;

TextImpl::TextImpl (TextImpl const& other)
  : doc_ptr {other.doc_ptr},
    node {other.node}
{}

TextImpl::TextImpl (TextImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)},
    node {std::move (other.node)}
{}

void
TextImpl::swap (TextImpl &other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
  node.swap (other.node);
}

// node impl

NodeImpl::NodeImpl (pugi::xml_document* d,
                    SwappableXmlNode n)
  : doc_ptr {d},
    node {std::move (n)}
{}

NodeImpl::~NodeImpl () noexcept = default;

NodeImpl::NodeImpl (NodeImpl const& other)
  : doc_ptr {other.doc_ptr},
    node {other.node}
{}

NodeImpl::NodeImpl (NodeImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)},
    node {std::move (other.node)}
{}

void
NodeImpl::swap (NodeImpl &other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
  node.swap (other.node);
}

// document impl

DocumentImpl::DocumentImpl (pugi::xml_document* d)
  : doc_ptr {d}
{}

DocumentImpl::~DocumentImpl () = default;

DocumentImpl::DocumentImpl (DocumentImpl const& other)
  : doc_ptr {other.doc_ptr}
{}

DocumentImpl::DocumentImpl (DocumentImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)}
{}

void DocumentImpl::swap (DocumentImpl &other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
}

// bundle impl

BundleImpl::BundleImpl ()
  : doc {std::make_unique<pugi::xml_document> ()}
{}

BundleImpl::~BundleImpl () = default;

BundleImpl::BundleImpl (BundleImpl&& other) noexcept
  : doc {std::move (other.doc)}
{}

BundleImpl&
BundleImpl::operator= (BundleImpl&& other) noexcept
{
  BundleImpl tmp {std::move (other)};

  swap (tmp);
  return *this;
}

void
BundleImpl::swap (BundleImpl &other) noexcept
{
  doc.swap (other.doc);
}

// misc

pugi::xml_node
add_child (pugi::xml_node& node,
           Type::StringView name)
{
  auto pnode = node.append_child (name.to_string ().c_str ());

  if (!pnode)
  {
    std::ostringstream oss;

    oss << "could not add a child named \"" << name << "\"";
    throw std::runtime_error {oss.str ()};
  }

  return pnode;
}

template <Helpers::WrapperType TypeV>
class ParentVariant
{
public:
  using ReturnType = typename Creator<TypeV>::NodeOrDocType;

  static ReturnType document(NodeImpl const& impl)
  {
    return ReturnType {Creator<TypeV>::create_document (DocumentImpl {impl.doc_ptr})};
  }

  static ReturnType node(NodeImpl const& impl, pugi::xml_node&& parent)
  {
    return ReturnType {Creator<TypeV>::create_node (NodeImpl {impl.doc_ptr, SwappableXmlNode {std::move (parent)}})};
  }
};

class ParentBasicImpl
{
public:
  using ReturnType = BasicNodeImpl;

  static ReturnType document(NodeImpl const& impl)
  {
    return ReturnType {impl.doc_ptr};
  }

  static ReturnType node(NodeImpl const& impl, pugi::xml_node&& parent)
  {
    return ReturnType {impl.doc_ptr, SwappableXmlNode {std::move (parent)}};
  }
};

template <typename ImplDetailsP>
inline typename ImplDetailsP::ReturnType
get_node_parent_tmpl (NodeImpl const& impl)
{
  auto p = impl.node.parent ();
  if (!p)
  {
    throw std::runtime_error ("arf");
  }

  switch (p.type ())
  {
  case pugi::node_document:
    return ImplDetails::document (impl);

  case pugi::node_element:
    return ImplDetails::node (impl, std::move (p));

  case pugi::node_pcdata:
  case pugi::node_cdata:
  case pugi::node_null:
  case pugi::node_comment:
  case pugi::node_pi:
  case pugi::node_declaration:
  case pugi::node_doctype:
    break;
  }

  throw std::runtime_error ("blah");
}

Type::Optional<NodeImpl>
get_child (pugi::xml_document* doc_ptr,
           pugi::xml_node const& node,
           Type::StringView name)
{
  auto c = node.child (name.to_string ().c_str ());

  if (c)
  {
    return Type::Optional<NodeImpl> {Type::OptionalInPlace{}, doc_ptr, SwappableXmlNode {std::move (c)}};
  }

  return Type::Optional<NodeImpl> {};
}

/*

class RangeDetailsLayout
{
public:
  static auto
  pugi_range (pugi::xml_node& node)
  {}

  static auto
  convert (pugi::xml_document* doc_ptr,
           pugi::xml_node& node)
  {}

  template <typename IteratorP>
  static auto
  filter (IteratorP iterator, IteratorP end)
  {}
};

*/

class PugiGetChildrenRange
{
public:
  auto
  operator() (pugi::xml_node const& node)
  {
    return node.children ();
  }
};

class PugiGetNamedChildrenRange
{
public:
  auto
  operator() (pugi::xml_node const& node,
              StringView const& name)
  {
    return node.children (name.to_string ().c_str ());
  }
};

template <typename PugiRangeDetailsP, typename ConverterP, typename PredicateP>
class PugiXmlNodeBasedRangeDetailsTmpl
{
public:
  template <typename ArgsP...>
  static auto
  pugi_range (pugi::xml_node const& node,
              ArgsP&&... args)
  {
    return PugiRangeDetailsP {} (node, args...);
  }

  static auto
  convert (pugi::xml_document* doc_ptr,
           pugi::xml_node const&)
  {
    return ConverterP {doc_ptr};
  }

  template <typename IteratorP>
  static auto
  filter (IteratorP iterator, IteratorP end)
  {
    return boost::make_filter_iterator (PredicateP {}, iterator, end);
  }
};

template <Helpers::WrapperType TypeV>
inline
NodeTransform::NodeTransform ()
  : doc {}
{}

template <Helpers::WrapperType TypeV>
inline
NodeTransform::NodeTransform (pugi::xml_document* d)
  : doc {d}
{}

template <Helpers::WrapperType TypeV>
inline typename Creator<TypeV>::NodeType
NodeTransform::operator() (pugi::xml_node const& n) const
{
  return Creator<TypeV>::create_node (NodeImpl {doc, SwappableXmlNode {n}});
}

inline bool
NodePredicate::operator() (pugi::xml_node const& node) const
{
  return node.type () == pugi::node_element;
}

template <typename Helpers::WrapperType TypeV>
using ChildrenRangeDetails = PugiXmlNodeBasedRangeDetailsTmpl<PugiGetChildrenRange, NodeTransform<TypeV>, NodePredicate>;
template <typename Helpers::WrapperType TypeV>
using NamedChildrenRangeDetails = PugiXmlNodeBasedRangeDetailsTmpl<PugiGetNamedChildrenRange, NodeTransform<TypeV>, NodePredicate>;

template <Helpers::WrapperType TypeV>
inline
AttributeTransform::AttributeTransform ()
  : doc_ptr {}
{}

template <Helpers::WrapperType TypeV>
inline
AttributeTransform::AttributeTransform (pugi::xml_document* d,
                                        pugi::xml_node const& n)
  : doc_ptr {d},
    parent {n}
{}

template <Helpers::WrapperType TypeV>
inline typename Creator<TypeV>::AttributeType
AttributeTransform::operator() (pugi::xml_attribute const& a) const
{
  return Creator<TypeV>::create_attribute (AttributeImpl {doc, SwappableXmlNode {parent}, SwappableXmlAttribute {a}});
}

template <typename ConverterP>
class AttributeRangeDetailsTmpl
{
public:
  static auto pugi_range (pugi::xml_node const& node)
  {
    return node.attributes ();
  }

  static auto
  convert (pugi::xml_document* doc_ptr,
           pugi::xml_node const& node)
  {
    return ConverterP {doc_ptr, node};
  }

  template <typename IteratorP>
  static auto
  filter (IteratorP iterator, IteratorP)
  {
    return iterator;
  }
};

template <typename Helpers::WrapperType TypeV>
using AttributesRangeDetails = AttributeRangeDetailsTmpl<AttributeTransform<TypeV>>;

template <Helpers::WrapperType TypeV>
inline
TextTransform::TextTransform ()
  : doc_ptr {}
{}

template <Helpers::WrapperType TypeV>
inline
TextTransform::TextTransform (pugi::xml_document* d)
  : doc_ptr {d}
{}

template <Helpers::WrapperType TypeV>
inline typename Helpers::Creator<TypeV>::TextType
TextTransform::operator() (pugi::xml_node const& n) const
{
  return Helpers::Creator<TypeV>::create_text (TextImpl {doc_ptr, SwappableXmlNode {n}});
}

inline bool
TextPredicate::operator() (pugi::xml_node const& node) const
{
  auto const type = node.type ();

  return (type == pugi::node_pcdata) || (type == pugi::node_cdata);
}

template <typename Helpers::WrapperType TypeV>
using TextsRangeDetails = PugiXmlNodeBasedRangeDetailsTmpl<PugiGetChildrenRange, TextTransform<TypeV>, TextPredicate>;

template <Helpers::WrapperType TypeV>
inline
NodeOrTextTransform::NodeOrTextTransform ()
  : doc_ptr {}
{}

template <Helpers::WrapperType TypeV>
inline
NodeOrTextTransform::NodeOrTextTransform (pugi::xml_document* d)
  : doc_ptr {d}
{}

template <Helpers::WrapperType TypeV>
inline typename Helpers::Creator<TypeV>::NodeOrTextType
NodeOrTextTransform::operator() (pugi::xml_node const& n) const
{
  if (NodePredicate {} (n))
  {
    return typename Helpers::Creator<TypeV>::NodeOrTextType {NodeTransform<TypeV> {doc_ptr} (n)};
  }
  if (TextPredicate {} (n))
  {
    return typename Helpers::Creator<TypeV>::NodeOrTextType {TextTransform<TypeV> {doc_ptr} (n)};
  }

  throw std::runtime_error ("Argh");
}

inline bool
NodeOrTextPredicate::operator() (pugi::xml_node const& node) const
{
  return NodePredicate {} (n) || TextPredicate {} (n);
}

template <typename Helpers::WrapperType TypeV>
using NodesOrTextsRangeDetails = PugiXmlNodeBasedRangeDetailsTmpl<PugiGetChildrenRange, NodeOrTextTransform<TypeV>, NodeOrTextPredicate>;

template <typename RangeDetailsP, typename... Args>
auto
to_range (pugi::xml_document* doc_ptr,
          pugi::xml_node const& node,
          Args&&... args)
{
  auto r = RangeDetailsP::pugi_range (node, args...);
  auto convert = RangeDetailsP::convert (doc, node);
  auto rb = r.begin ();
  auto re = r.end ();
  auto fb = RangeDetailsP::filter (rb, re);
  auto fe = RangeDetailsP::filter (re, re);
  auto b = boost::make_transform_iterator (fb, convert);
  auto e = boost::make_transform_iterator (fe, convert);

  return boost::make_iterator_range (b, e);
}

Type::Optional<AttributeImpl>
get_attribute (pugi::xml_document* doc_ptr,
               pugi::xml_node const& parent,
               Type::StringView name)
{
  auto a = node.attribute (name.to_string ().c_str ());

  if (a)
  {
    return Type::Optional<AttributeImpl> {Type::OptionalInPlace{}, doc_ptr, SwappableXmlNode {std::move (c)}, SwappableXmlAttribute {std::move (a)}};
  }

  return Type::Optional<AttributeImpl> {};
}

pugi::xml_node_type
text_type_to_pugi_node_type (TextType text_type)
{
  switch (text_type)
  {
  case TextType::Parsed:
    return pugi::node_pcdata;

  case TextType::Raw:
    return pugi::node_cdata;
  }

  throw std::runtime_error {"argh"};
}

TextType
pugi_node_type_to_text_type (pugi::xml_node_type node_type)
{
  switch (node_type)
  {
  case pugi::node_pcdata:
    return TextType::Parsed;

  case pugi::node_cdata:
    return TextType::Raw;

  case pugi::node_null:
  case pugi::node_document:
  case pugi::node_element:
  case pugi::node_comment:
  case pugi::node_pi:
  case pugi::node_declaration:
  case pugi::node_doctype:
    break;
  }

  throw std::runtime_error {"argh"};
}

} // namespace PugiXmlDetails

// basic node methods

template <>
/* static */ inline Type::Wrapper<BasicNode>
BasicNode::create (XmlImpl::BasicNodeImpl i)
{
  return Type::Wrapper<BasicNode> {std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<BasicNode const>
BasicNode::create_const (XmlImpl::BasicNodeImpl i)
{
  return Type::Wrapper<BasicNode const> {std::move (i)};
}

template <>
inline Type::Wrapper<Document>
BasicNode::document ()
{
  return Document::create (DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::Wrapper<Document const>
BasicNode::document () const
{
  return Document::create_const (DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::Optional<Type::Wrapper<Node>>
BasicNode::child (Type::StringView name)
{
  auto child_impl = PugiXmlDetails::get_child (impl.doc_ptr, impl.get_node (), name);

  if (child_impl)
  {
    return Type::Optional<Type::Wrapper<Node>> {Type::OptionalInPlace{}, *child_impl};
  }

  return Type::Optional<Type::Wrapper<Node>> {};
}

template <>
inline Type::Optional<Type::Wrapper<Node const>>
BasicNode::child (Type::StringView name) const
{
  auto child_impl = PugiXmlDetails::get_child (impl.doc_ptr, impl.get_node (), name);

  if (child_impl)
  {
    return Type::Optional<Type::Wrapper<Node const>> {Type::OptionalInPlace{}, std::move (*child_impl)};
  }

  return Type::Optional<Type::Wrapper<Node const>> {};
}

template <>
inline XmlImpl::NodeRange
BasicNode::children ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Helpers::WrapperType::Mutable>> (impl.doc_ptr, impl.get_node ());
}

template <>
inline XmlImpl::NodeConstRange
BasicNode::children () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Helpers::WrapperType::Const>> (impl.doc_ptr, impl.get_node ());
}

template <>
inline XmlImpl::NodeRange
BasicNode::children (Type::StringView name)
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NamedChildrenRangeDetails<Helpers::WrapperType::Mutable>> (impl.doc_ptr, impl.get_node (), name);
}

template <>
inline XmlImpl::NodeConstRange
BasicNode::children (Type::StringView name) const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NamedChildrenRangeDetails<Helpers::WrapperType::Const>> (impl.doc_ptr, impl.get_node (), name);
}

template <>
inline void
BasicNode::remove (Type::Wrapper<Node> const& node)
{
  impl.get_node ().remove_child (node->impl.node);
}

template <>
inline
BasicNode::BasicNodeTmpl (XmlImpl::BasicNodeImpl i)
  : impl {std::move (i)}
{}

template <>
inline
BasicNode::BasicNodeTmpl (BasicNode const& other)
  : impl {other.impl}
{}

template <>
inline
BasicNode::BasicNodeTmpl (BasicNode&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
inline
BasicNode::~BasicNodeTmpl () noexcept\
{}

template <>
inline BasicNode&
BasicNode::operator= (BasicNode const& other)
{
  BasicNode tmp {other};

  swap (tmp);
  return *this;
}

template <>
inline BasicNode&
BasicNode::operator= (BasicNode&& other) noexcept
{
  BasicNode tmp {std::move (other)};

  swap (tmp);
  return *this;
}

template <>
inline void
BasicNode::swap (BasicNode& other) noexcept
{
  impl.swap (other.impl);
}

// attribute methods

template <>
/* static */ inline Type::Wrapper<Attribute>
Attribute::create (XmlImpl::AttributeImpl i)
{
  return Type::Wrapper<Attribute> {std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Attribute const>
Attribute::create_const (XmlImpl::AttributeImpl i)
{
  return Type::Wrapper<Attribute const> {std::move (i)};
}

template <>
inline Type::Wrapper<Document>
Attribute::document ()
{
  return Document::create (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::Wrapper<Document const>
Attribute::document () const
{
  return Document::create_const (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::StringView
Attribute::name () const
{
  return impl.attribute.name ();
}

template <>
inline void
Attribute::set_name (Type::StringView name)
{
  impl.attribute.set_name (name.to_string ().c_str ());
}

template <>
inline Type::StringView
Attribute::value () const
{
  return impl.attribute.value ();
}

template <>
inline void
Attribute::set_value (Type::StringView value)
{
  impl.attribute.set_value (value.to_string ().c_str ());
}

template <>
inline Type::Wrapper<Node>
Attribute::parent ()
{
  return Node::create (NodeImpl {impl.doc_ptr, SwappableXmlNode {impl.parent}});
}

template <>
inline Type::Wrapper<Node const>
Attribute::parent () const
{
  return Node::create_const (NodeImpl {impl.doc_ptr, SwappableXmlNode {impl.parent}});
}

template <>
inline
Attribute::AttributeTmpl (XmlImpl::AttributeImpl i)
  : impl {std::move (i)}
{}

template <>
inline
Attribute::AttributeTmpl (Attribute const& other)
  : impl {other.impl}
{}

template <>
inline
Attribute::AttributeTmpl (Attribute&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
inline
Attribute::~AttributeTmpl () noexcept
{}

template <>
inline Attribute&
Attribute::operator= (Attribute const& other)
{
  Attribute tmp {other};

  swap (tmp);
  return *this;
}

template <>
inline Attribute&
Attribute::operator= (Attribute&& other) noexcept
{
  Attribute tmp {std::move (other)};

  swap (tmp);
  return *this;
}

template <>
inline void
Attribute::swap (Attribute& other) noexcept
{
  impl.swap (other.impl);
}

// text methods

template <>
/* static */ inline Type::Wrapper<Text>
Text::create (XmlImpl::TextImpl i)
{
  return Type::Wrapper<Text> {std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Text const>
Text::create_const (XmlImpl::TextImpl i)
{
  return Type::Wrapper<Text const> {std::move (i)};
}

template <>
inline Type::Wrapper<Document>
Text::document ()
{
  return Document::create (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::Wrapper<Document const>
Text::document () const
{
  return Document::create_const (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::StringView
Text::text () const
{
  return impl.node.value ();
}

template <>
inline void
Text::set_text (Type::StringView text)
{
  impl.node.set_value (text.to_string ().c_str ());
}

template <>
inline Type::Wrapper<Node>
Text::parent ()
{
  return Node::create (NodeImpl {impl.doc_ptr, SwappableXmlNode {impl.node.parent ()}});
}

template <>
inline Type::Wrapper<Node const>
Text::parent () const
{
  return Node::create_const (NodeImpl {impl.doc_ptr, SwappableXmlNode {impl.node.parent ()}});
}

template <>
inline TextType
Text::type () const
{
  return pugi_node_type_to_text_type (impl.node.type ());
}

template <>
inline
Text::TextTmpl (XmlImpl::TextImpl i)
  : impl {std::move (i)}
{}

template <>
inline
Text::TextTmpl (TextTmpl const& other)
  : impl {other.impl}
{}

template <>
inline
Text::TextTmpl (TextTmpl&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
inline
Text::~TextTmpl () noexcept
{}

template <>
inline Text&
Text::operator= (Text const& other)
{
  Text tmp {other};

  swap (tmp);
  return *this;
}

template <>
inline Text&
Text::operator= (Text&& other) noexcept
{
  Text tmp {std::move (other)};

  swap (tmp);
  return *this;
}

template <>
inline void
Text::swap (Text& other) noexcept
{
  impl.swap (other.impl);
}

// node methods

template <>
/* static */ inline Type::Wrapper<NodeTmpl>
Node::create (XmlImpl::NodeImpl i)
{
  return Type::Wrapper<Node> {std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<NodeTmpl const>
Node::create_const (XmlImpl::NodeImpl i)
{
  return Type::Wrapper<Node const> {std::move (i)};
}

template <>
inline Type::Wrapper<BasicNode>
Node::as_basic_node ()
{
  return BasicNode::create (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr, impl.node});
}

template <>
inline Type::Wrapper<BasicNode const>
Node::as_basic_node () const
{
  return BasicNode::create_const (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr, impl.node});
}

template <>
inline Type::Wrapper<Document>
Node::document ()
{
  return Document::create (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::Wrapper<Document const>
Node::document () const
{
  return Document::create_const (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::StringView
Node::name () const
{
  return impl.node.name ();
}

template <>
inline NodeOrDoc
Node::parent ()
{
  return get_node_parent_tmpl<PugiXmlDetails::ParentVariant<Helpers::WrapperType::Mutable>> (impl);
}

template <>
inline NodeOrDocConst
Node::parent () const
{
  return get_node_parent_tmpl<PugiXmlDetails::ParentVariant<Helpers::WrapperType::Const>> (impl);
}

template <>
inline Type::Wrapper<BasicNode>
Node::basic_parent ()
{
  return BasicNode::create (get_node_parent_tmpl<PugiXmlDetails::ParentBasicImpl> (impl));
}

template <>
inline Type::Wrapper<BasicNode const>
Node::basic_parent () const
{
  return BasicNode::create_const (get_node_parent_tmpl<PugiXmlDetails::ParentBasicImpl> (impl));
}

template <>
inline Type::Optional<Type::Wrapper<Node>>
Node::child (Type::StringView name)
{
  auto child_impl = PugiXmlDetails::get_child (impl.doc_ptr, impl.node, name);

  if (child_impl)
  {
    return Type::Optional<Type::Wrapper<Node>> {Type::OptionalInPlace{}, std::move (*child_impl)};
  }

  return Type::Optional<Type::Wrapper<Node>> {};
}

template <>
inline Type::Optional<Type::Wrapper<Node const>>
Node::child (Type::StringView name) const
{
  auto child_impl = PugiXmlDetails::get_child (impl.doc_ptr, impl.node, name);

  if (child_impl)
  {
    return Type::Optional<Type::Wrapper<Node const>> {Type::OptionalInPlace{}, std::move (*child_impl)};
  }

  return Type::Optional<Type::Wrapper<Node const>> {};
}

template <>
inline XmlImpl::NodeRange
Node::children ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Helpers::WrapperType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::NodeConstRange
Node::children () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Helpers::WrapperType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::NodeRange
Node::children (Type::StringView name)
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NamedChildrenRangeDetails<Helpers::WrapperType::Mutable>> (impl.doc_ptr, impl.node, name);
}

template <>
inline XmlImpl::NodeConstRange
Node::children (Type::StringView name) const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NamedChildrenRangeDetails<Helpers::WrapperType::Const>> (impl.doc_ptr, impl.node, name);
}

template <>
inline Type::Optional<Type::Wrapper<Attribute>>
Node::attribute (StringView name)
{
  auto attr_impl = PugiXmlDetails::get_attribute (impl.doc_ptr, impl.node, name);

  if (attr_impl)
  {
    return Type::Optional<Type::Wrapper<Attribute>> {Type::OptionalInPlace{}, *attr_impl};
  }

  return Type::Optional<Type::Wrapper<Attribute>> {};
}

template <>
inline Type::Optional<Type::Wrapper<Attribute const>>
Node::attribute (StringView name) const
{
  auto attr_impl = PugiXmlDetails::get_attribute (impl.doc_ptr, impl.node, name);

  if (attr_impl)
  {
    return Type::Optional<Type::Wrapper<Attribute const>> {Type::OptionalInPlace{}, *attr_impl};
  }

  return Type::Optional<Type::Wrapper<Attribute const>> {};
}

template <>
inline XmlImpl::AttributeRange
Node::attributes ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::AttributesRangeDetails<Helpers::WrapperType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::AttributeConstRange
Node::attributes () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::AttributesRangeDetails<Helpers::WrapperType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::TextRange
Node::texts ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::TextsRangeDetails<Helpers::WrapperType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::TextConstRange
Node::texts () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::TextsRangeDetails<Helpers::WrapperType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::NodeOrTextRange
Node::all ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NodesOrTextsRangeDetails<Helpers::WrapperType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::NodeOrTextConstRange
Node::all () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NodesOrTextsRangeDetails<Helpers::WrapperType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline Type::Wrapper<Node>
Node::add_child (Type::StringView name)
{
  return Node::create (PugiXmlDetails::NodeImpl {impl.doc_ptr, PugiXmlDetails::add_child (impl.node, name)});
}

template <>
inline Type::Wrapper<Attribute>
Node::add_attribute (Type::StringView name)
{
  auto pattr = impl.node.append_attribute (name.to_string ().c_str ());

  if (!pattr)
  {
    std::ostringstream oss;

    oss << "could not add an attribute named \"" << name << "\" with value \"" << value << "\"";
    throw std::runtime_error {oss.str ()};
  }

  return Attribute::create (PugiXmlDetails::AttributeImpl {impl.doc_ptr, impl.node, SwappableXmlAttribute {std::move (pattr)}});
}

template <>
inline Type::Wrapper<Text>
Node::add_text (TextType text_type = TextType::Parsed)
{
  auto pnode = node.append_child (text_type_to_pugi_node_type (text_type));

  if (!pnode)
  {
    throw std::runtime_error {"could not add a text child"};
  }

  return Text::create (TextImpl {impl.doc_ptr, SwappableXmlNode {std::move (pnode)}});
}

template <>
inline void
Node::remove (Type::Wrapper<Node> const& node)
{
  impl.node.remove_child (node->impl.node);
}

template <>
inline void
Node::remove (Type::Wrapper<Attribute> const& attribute)
{
  impl.node.remove_attribute (attribute->impl.attr);
}

template <>
inline void
Node::remove (Type::Wrapper<Text> const& text)
{
  impl.node.remove_child (text->impl.node);
}

template <>
inline
Node::NodeTmpl (XmlImpl::NodeImpl i)
  : impl {std::move (i)}
{}

template <>
inline
Node::NodeTmpl (Node const& other)
  : impl {other.impl}
{}

template <>
inline
Node::NodeTmpl (Node&& other)
  : impl {std::move (other.impl)}
{}

template <>
inline
Node::~NodeTmpl () noexcept
{}

template <>
inline Node&
Node::operator= (Node const& other)
{
  Node tmp {other};

  swap (tmp);
  return *this;
}

template <>
inline Node&
Node::operator= (Node&& other) noexcept
{
  Node tmp {std::move (other)};

  swap (tmp);
  return *this;
}

template <>
inline void
Node::swap (Node& other) noexcept
{
  impl.swap (other.impl);
}

// document methods

template <>
/* static */ inline Type::Wrapper<DocumentTmpl>
Document::create (XmlImpl::DocumentImpl i)
{
  return Type::Wrapper<Document> {std::move (i)};
}

template <>
/* static */
Type::Wrapper<DocumentTmpl const>
Document::create_const (XmlImpl::DocumentImpl i)
{
  return Type::Wrapper<Document const> {std::move (i)};
}

template <>
inline Type::Wrapper<BasicNode>
Document::as_basic_node ()
{
  return BasicNode::create (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr});
}

template <>
inline Type::Wrapper<BasicNode const>
Document::as_basic_node () const
{
  return BasicNode::create_const (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr});
}

template <>
inline Type::Optional<Type::Wrapper<Node>>
Document::root_tag ()
{
  auto tag = impl.doc_ptr->document_element ();
  if (tag)
  {
    return Type::Optional<Type::Wrapper<Node>> {Type::OptionalInPlace{}, PugiXmlDetails::NodeImpl {impl.doc_ptr, std::move (tag)}};
  }

  return Type::Optional<Type::Wrapper<Node>> {};
}

template <>
inline Type::Optional<Type::Wrapper<Node const>>
Document::root_tag ()
{
  auto tag = impl.doc_ptr->document_element ();
  if (tag)
  {
    return Type::Optional<Type::Wrapper<Node const>> {Type::OptionalInPlace{}, PugiXmlDetails::NodeImpl {impl.doc_ptr, std::move (tag)}};
  }

  return Type::Optional<Type::Wrapper<Node const>> {};
}

template <>
inline Type::Wrapper<Node>
Document::add_root (Type::StringView name)
{
  {
    auto tag = root_tag ();
    if (tag)
    {
      std::ostringstream oss;

      oss << "document already has a root \"" << tag->name () << "\"";
      throw std::runtime_error (oss.str ());
    }
  }

  return Node::create (PugiXmlDetails::NodeImpl {impl.doc_ptr, PugiXmlDetails::add_child (*impl.doc_ptr, name)});
}

template <>
inline
Document::DocumentTmpl (XmlImpl::DocumentImpl i)
  : impl {std::move (i)}
{}

template <>
inline
Document::DocumentTmpl (Document const& other)
  : impl {other.impl}
{}

template <>
inline
Document::DocumentTmpl (Document&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
inline
Document::~Document () noexcept
{}

template <>
inline Document&
Document::operator= (Document const& other)
{
  Document tmp {other};

  swap (tmp);
  return *this;
}

template <>
inline Document&
Document::operator= (Document&& other) noexcept
{
  Document tmp {std::move (other)};

  swap (tmp);
  return *this;
}

// bundle methods

template <>
inline
Bundle::BundleTmpl ()
  : impl {}
{}

template <>
inline
Bundle::BundleTmpl (Type::StringView filename)
  : Bundle {}
{
  auto result = impl.doc->load_file (filename.to_string ().c_str ());

  if (!result)
  {
    std::ostringstream oss;

    oss << "Failed to parse '" << filename << "': " << result.description ();
    throw ParseError {oss.str ()};
  }
}

template <>
inline
Bundle::~BundleTmpl () noexcept
{}

template <>
inline
Bundle::BundleTmpl (Bundle&& other) noexcept
  : impl {std::move (doc.impl)}
{}

template <>
inline Bundle&
Bundle::operator= (Bundle&& other) noexcept
{
  Bundle tmp {std::move (doc)};

  swap (tmp);
  return *this;
}

template <>
void
Bundle::swap (Bundle& other) noexcept
{
  impl.swap (other.impl);
}

template <>
inline Type::Wrapper<Document>
Bundle::document ()
{
  return Document::create (PugiXmlDetails::DocumentImpl {impl.get ()});
}

template <>
inline Type::Wrapper<Document const>
Bundle::document () const
{
  return Document::create_const (PugiXmlDetails::DocumentImpl {impl.get ()});
}

// walker

template <typename ImplP>
class ConstWalkerTypeTmpl
{
public:
  using Document = Type::Wrapper<DocumentTmpl<ImplP> const>;
  using Node = Type::Wrapper<NodeTmpl<ImplP> const>;
  using Text = Type::Wrapper<TextTmpl<ImplP> const>;
  using NodeOrDoc = NodeOrDocConstTmpl<ImplP>;
  using Impl = ImplP::ConstWalkerTypeImpl;
};

template <typename ImplP>
class MutWalkerTypeTmpl
{
public:
  using Document = Type::Wrapper<DocumentTmpl<ImplP>>;
  using Node = Type::Wrapper<NodeTmpl<ImplP>>;
  using Text = Type::Wrapper<TextTmpl<ImplP>>;
  using NodeOrDoc = NodeOrDocTmpl<ImplP>;
  using Impl = ImplP::MutWalkerTypeImpl;
};

template <typename WalkerTypeP>
WalkerTmpl::WalkerTmpl ()
{}

template <typename WalkerTypeP>
void
WalkerTmpl::walk (typename WalkerTypeP::NodeOrDoc node_or_doc) const
{
  typename WalkerTypeP::Impl host {this, node_or_doc};
  auto pugi_walker = host.walker ();
  auto visitor = host.visitor ();

  boost::apply_visitor (visitor, node_or_doc).traverse (pugi_walker);
}

/*
virtual bool doc (typename WalkerTypeP::Document& doc, int depth) const= 0;
virtual bool node (typename WalkerTypeP::Node& node, int depth) const = 0;
virtual bool text (typename WalkerTypeP::Text& text, int depth) const = 0;
virtual bool postprocess_node (typename WalkerTypeP::Node& node, int depth) const = 0;
virtual bool postprocess_doc (typename WalkerTypeP::Document& doc, int depth) const = 0;
*/

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_XML_PUGI_HH
