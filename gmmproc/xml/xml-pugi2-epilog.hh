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

// impls

// basic node

class BasicNodeImpl
{
public:
  BasicNodeImpl (pugi::xml_document* d);
  BasicNodeImpl (pugi::xml_document* d,
                 pugi::xml_node n);
  ~BasicNodeImpl () noexcept;

  BasicNodeImpl (BasicNodeImpl const& other);
  BasicNodeImpl (BasicNodeImpl&& other) noexcept;

  void swap (BasicNodeImpl& other) noexcept;

  bool equal (BasicNodeImpl const& other) const;

  pugi::xml_node get_node () const;

  pugi::xml_document* doc_ptr;
  Type::Variant<pugi::xml_document*, pugi::xml_node> doc_or_node;
};

// attribute

class AttributeImpl
{
public:
  AttributeImpl (pugi::xml_document* d,
                 pugi::xml_node p,
                 pugi::xml_attribute a);
  ~AttributeImpl () noexcept;

  AttributeImpl (AttributeImpl const& other);
  AttributeImpl (AttributeImpl&& other) noexcept;

  void swap (AttributeImpl& other) noexcept;

  bool equal (AttributeImpl const& other) const;

  pugi::xml_document* doc_ptr;
  pugi::xml_node parent;
  pugi::xml_attribute attribute;
};

// text

class TextImpl
{
public:
  TextImpl (pugi::xml_document* d,
            pugi::xml_node n);
  ~TextImpl () noexcept;

  TextImpl (TextImpl const& other);
  TextImpl (TextImpl&& other) noexcept;

  void swap (TextImpl& other) noexcept;

  bool equal (TextImpl const& other) const;

  pugi::xml_document* doc_ptr;
  pugi::xml_node node;
};

// node

class NodeImpl
{
public:
  NodeImpl (pugi::xml_document* d,
            pugi::xml_node n);
  ~NodeImpl () noexcept;

  NodeImpl (NodeImpl const& other);
  NodeImpl (NodeImpl&& other) noexcept;

  void swap (NodeImpl &other) noexcept;

  pugi::xml_document* doc_ptr;
  pugi::xml_node node;
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

  std::unique_ptr<pugi::xml_document> doc;
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
  pugi::xml_document* doc_ptr;
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
  pugi::xml_document* doc_ptr;
};

class NodeOrTextPredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

template <Helpers::WrapperType TypeV>
class WalkerContext
{
public:
  WalkerContext (Walker<TypeV> const* w, typename Helpers::Creator<TypeV>::NodeOrDocType nod);

  void walk ();

private:
  class PugiWalker;
  class NodeOrDocForDocVisitor;
  class NodeOrDocForNodeVisitor;

  static pugi::xml_document* get_doc_from_variant (typename Helpers::Creator<TypeV>::NodeOrDocType node_or_doc);
  static pugi::xml_node get_node_from_variant (typename Helpers::Creator<TypeV>::NodeOrDocType node_or_doc);

  Walker<TypeV> const* walker;
  pugi::xml_document* doc;
  pugi::xml_node node;
};

template <Helpers::WrapperType TypeV>
class WalkerContext<TypeV>::PugiWalker : public pugi::xml_tree_walker
{
public:
  PugiWalker (WalkerContext<TypeV> const* h,
              pugi::xml_document* d);

private:
  virtual bool for_each (pugi::xml_node& node) override;
  virtual bool end (pugi::xml_node& node) override;

  bool run_postprocessing ();

  WalkerContext<TypeV> const* ctx;
  pugi::xml_document* doc;
  std::stack<std::pair<Node, int>> nodes;
  bool visited_doc;
  int doc_depth;
};

template <Helpers::WrapperType TypeV>
class WalkerContext<TypeV>::NodeOrDocForDocVisitor : public boost::static_visitor<pugi::xml_document*>
{
public:
  NodeOrDocForDocVisitor ();

  template <typename NodeOrDocP>
  pugi::xml_document*
  operator() (NodeOrDocP const& node_or_doc) const;
};

template <Helpers::WrapperType TypeV>
class WalkerContext<TypeV>::NodeOrDocForNodeVisitor : public boost::static_visitor<pugi::xml_node>
{
public:
  NodeOrDocForNodeVisitor ();

  template <typename NodeOrDocP>
  pugi::xml_node
  operator() (NodeOrDocP const& node_or_doc) const;
};

template <typename TransformP, typename IteratorP>
using TransformRange = boost::iterator_range<boost::transform_iterator<TransformP, IteratorP>>;

template <typename TransformP, typename PredicateP>
using NodeTransformFilterRange = TransformRange<TransformP,
                                                boost::filter_iterator<PredicateP,
                                                                       pugi::xml_node_iterator>>;

} // namespace PugiXmlDetails

class XmlImpl
{
public:
  using BasicNodeImpl = PugiXmlDetails::BasicNodeImpl;
  using AttributeImpl = PugiXmlDetails::AttributeImpl;
  using TextImpl = PugiXmlDetails::TextImpl;
  using NodeImpl = PugiXmlDetails::NodeImpl;
  using DocumentImpl = PugiXmlDetails::DocumentImpl;
  using BundleImpl = PugiXmlDetails::BundleImpl;

  using NodeRange = PugiXmlDetails::NodeTransformFilterRange<PugiXmlDetails::NodeTransform<Helpers::WrapperType::Mutable>, PugiXmlDetails::NodePredicate>;
  using NodeConstRange = PugiXmlDetails::NodeTransformFilterRange<PugiXmlDetails::NodeTransform<Helpers::WrapperType::Const>, PugiXmlDetails::NodePredicate>;
  using AttributeRange = PugiXmlDetails::TransformRange<PugiXmlDetails::AttributeTransform<Helpers::WrapperType::Mutable>, pugi::xml_attribute_iterator>;
  using AttributeConstRange = PugiXmlDetails::TransformRange<PugiXmlDetails::AttributeTransform<Helpers::WrapperType::Const>, pugi::xml_attribute_iterator>;
  using TextRange = PugiXmlDetails::NodeTransformFilterRange<PugiXmlDetails::TextTransform<Helpers::WrapperType::Mutable>, PugiXmlDetails::TextPredicate>;
  using TextConstRange = PugiXmlDetails::NodeTransformFilterRange<PugiXmlDetails::TextTransform<Helpers::WrapperType::Const>, PugiXmlDetails::TextPredicate>;
  using NodeOrTextRange = PugiXmlDetails::NodeTransformFilterRange<PugiXmlDetails::NodeOrTextTransform<Helpers::WrapperType::Mutable>, PugiXmlDetails::NodeOrTextPredicate>;
  using NodeOrTextConstRange = PugiXmlDetails::NodeTransformFilterRange<PugiXmlDetails::NodeOrTextTransform<Helpers::WrapperType::Const>, PugiXmlDetails::NodeOrTextPredicate>;

  template <Helpers::WrapperType TypeV>
  using WalkerImpl = PugiXmlDetails::WalkerContext<TypeV>;
};

// creators

template <>
/* static */ inline Type::Wrapper<BasicNode>
BasicNode::create (XmlImpl::BasicNodeImpl i)
{
  return Type::Wrapper<BasicNode> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<BasicNode const>
BasicNode::create_const (XmlImpl::BasicNodeImpl i)
{
  return Type::Wrapper<BasicNode const> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Attribute>
Attribute::create (XmlImpl::AttributeImpl i)
{
  return Type::Wrapper<Attribute> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Attribute const>
Attribute::create_const (XmlImpl::AttributeImpl i)
{
  return Type::Wrapper<Attribute const> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Text>
Text::create (XmlImpl::TextImpl i)
{
  return Type::Wrapper<Text> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Text const>
Text::create_const (XmlImpl::TextImpl i)
{
  return Type::Wrapper<Text const> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Node>
Node::create (XmlImpl::NodeImpl i)
{
  return Type::Wrapper<Node> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Node const>
Node::create_const (XmlImpl::NodeImpl i)
{
  return Type::Wrapper<Node const> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Document>
Document::create (XmlImpl::DocumentImpl i)
{
  return Type::Wrapper<Document> {Type::WrapperInPlace {}, std::move (i)};
}

template <>
/* static */ inline Type::Wrapper<Document const>
Document::create_const (XmlImpl::DocumentImpl i)
{
  return Type::Wrapper<Document const> {Type::WrapperInPlace {}, std::move (i)};
}

namespace PugiXmlDetails
{

// basic node impl

class BasicNodeImplNodeVisitor : public boost::static_visitor<pugi::xml_node>
{
public:
  pugi::xml_node
  operator() (pugi::xml_node const& node) const
  {
    return node;
  }

  pugi::xml_node
  operator() (pugi::xml_document const* doc_ptr) const
  {
    return doc_ptr->root ();
  }
};

inline
BasicNodeImpl::BasicNodeImpl (pugi::xml_document* d)
  : doc_ptr {d},
    doc_or_node {d}
{}

inline
BasicNodeImpl::BasicNodeImpl (pugi::xml_document* d,
                              pugi::xml_node n)
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

inline bool
BasicNodeImpl::equal (BasicNodeImpl const& other) const
{
  return std::tie (doc_ptr, doc_or_node) == std::tie (other.doc_ptr, other.doc_or_node);
}

inline pugi::xml_node
BasicNodeImpl::get_node () const
{
  return boost::apply_visitor (BasicNodeImplNodeVisitor {}, doc_or_node);
}

// attribute impl

inline
AttributeImpl::AttributeImpl (pugi::xml_document* d,
                              pugi::xml_node p,
                              pugi::xml_attribute a)
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

inline
AttributeImpl::AttributeImpl (AttributeImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)},
    parent {std::move (other.parent)},
    attribute {std::move (other.attribute)}
{}

inline void
AttributeImpl::swap (AttributeImpl& other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
  swap (parent, other.parent);
  swap (attribute, other.attribute);
}

inline bool
AttributeImpl::equal (AttributeImpl const& other) const
{
  return std::tie (doc_ptr, parent, attribute) == (std::tie (other.doc_ptr, other.parent, other.attribute));
}

// text impl

inline
TextImpl::TextImpl (pugi::xml_document* d,
                    pugi::xml_node n)
  : doc_ptr {d},
    node {std::move (n)}
{}

inline
TextImpl::~TextImpl () noexcept = default;

inline
TextImpl::TextImpl (TextImpl const& other)
  : doc_ptr {other.doc_ptr},
    node {other.node}
{}

inline
TextImpl::TextImpl (TextImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)},
    node {std::move (other.node)}
{}

inline void
TextImpl::swap (TextImpl &other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
  swap (node, other.node);
}

inline bool
TextImpl::equal (TextImpl const& other) const
{
  return std::tie (doc_ptr, node) == std::tie (other.doc_ptr, other.node);
}

// node impl

inline
NodeImpl::NodeImpl (pugi::xml_document* d,
                    pugi::xml_node n)
  : doc_ptr {d},
    node {std::move (n)}
{}

inline
NodeImpl::~NodeImpl () noexcept = default;

inline
NodeImpl::NodeImpl (NodeImpl const& other)
  : doc_ptr {other.doc_ptr},
    node {other.node}
{}

inline
NodeImpl::NodeImpl (NodeImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)},
    node {std::move (other.node)}
{}

inline void
NodeImpl::swap (NodeImpl &other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
  swap (node, other.node);
}

// document impl

inline
DocumentImpl::DocumentImpl (pugi::xml_document* d)
  : doc_ptr {d}
{}

inline
DocumentImpl::~DocumentImpl () = default;

inline
DocumentImpl::DocumentImpl (DocumentImpl const& other)
  : doc_ptr {other.doc_ptr}
{}

inline
DocumentImpl::DocumentImpl (DocumentImpl&& other) noexcept
  : doc_ptr {std::move (other.doc_ptr)}
{}

inline void
DocumentImpl::swap (DocumentImpl &other) noexcept
{
  using std::swap;

  swap (doc_ptr, other.doc_ptr);
}

// bundle impl

inline
BundleImpl::BundleImpl ()
  : doc {std::make_unique<pugi::xml_document> ()}
{}

inline
BundleImpl::~BundleImpl () = default;

inline
BundleImpl::BundleImpl (BundleImpl&& other) noexcept
  : doc {std::move (other.doc)}
{}

inline void
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
  using ReturnType = typename Helpers::Creator<TypeV>::NodeOrDocType;

  static ReturnType document(NodeImpl const& impl)
  {
    return ReturnType {Helpers::Creator<TypeV>::create_document (DocumentImpl {impl.doc_ptr})};
  }

  static ReturnType node(NodeImpl const& impl, pugi::xml_node&& parent)
  {
    return ReturnType {Helpers::Creator<TypeV>::create_node (NodeImpl {impl.doc_ptr, std::move (parent)})};
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
    return ReturnType {impl.doc_ptr, std::move (parent)};
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
    return ImplDetailsP::document (impl);

  case pugi::node_element:
    return ImplDetailsP::node (impl, std::move (p));

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
    return Type::Optional<NodeImpl> {Type::OptionalInPlace{}, doc_ptr, std::move (c)};
  }

  return Type::Optional<NodeImpl> {};
}

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
              Type::StringView const& name)
  {
    return node.children (name.to_string ().c_str ());
  }
};

template <typename PugiRangeDetailsP, typename ConverterP, typename PredicateP>
class PugiXmlNodeBasedRangeDetailsTmpl
{
public:
  template <typename... ArgsP>
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
NodeTransform<TypeV>::NodeTransform ()
  : doc {}
{}

template <Helpers::WrapperType TypeV>
inline
NodeTransform<TypeV>::NodeTransform (pugi::xml_document* d)
  : doc {d}
{}

template <Helpers::WrapperType TypeV>
inline typename Helpers::Creator<TypeV>::NodeType
NodeTransform<TypeV>::operator() (pugi::xml_node const& n) const
{
  return Helpers::Creator<TypeV>::create_node (NodeImpl {doc, n});
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
AttributeTransform<TypeV>::AttributeTransform ()
  : doc_ptr {}
{}

template <Helpers::WrapperType TypeV>
inline
AttributeTransform<TypeV>::AttributeTransform (pugi::xml_document* d,
                                               pugi::xml_node const& n)
  : doc_ptr {d},
    parent {n}
{}

template <Helpers::WrapperType TypeV>
inline typename Helpers::Creator<TypeV>::AttributeType
AttributeTransform<TypeV>::operator() (pugi::xml_attribute const& a) const
{
  return Helpers::Creator<TypeV>::create_attribute (AttributeImpl {doc_ptr, parent, a});
}

template <typename ConverterP>
class AttributeRangeDetailsTmpl
{
public:
  static auto
  pugi_range (pugi::xml_node const& node)
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
TextTransform<TypeV>::TextTransform ()
  : doc_ptr {}
{}

template <Helpers::WrapperType TypeV>
inline
TextTransform<TypeV>::TextTransform (pugi::xml_document* d)
  : doc_ptr {d}
{}

template <Helpers::WrapperType TypeV>
inline typename Helpers::Creator<TypeV>::TextType
TextTransform<TypeV>::operator() (pugi::xml_node const& n) const
{
  return Helpers::Creator<TypeV>::create_text (TextImpl {doc_ptr, n});
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
NodeOrTextTransform<TypeV>::NodeOrTextTransform ()
  : doc_ptr {}
{}

template <Helpers::WrapperType TypeV>
inline
NodeOrTextTransform<TypeV>::NodeOrTextTransform (pugi::xml_document* d)
  : doc_ptr {d}
{}

template <Helpers::WrapperType TypeV>
inline typename Helpers::Creator<TypeV>::NodeOrTextType
NodeOrTextTransform<TypeV>::operator() (pugi::xml_node const& n) const
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
  return NodePredicate {} (node) || TextPredicate {} (node);
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
  auto convert = RangeDetailsP::convert (doc_ptr, node);
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
  auto a = parent.attribute (name.to_string ().c_str ());

  if (a)
  {
    return Type::Optional<AttributeImpl> {Type::OptionalInPlace{}, doc_ptr, parent, std::move (a)};
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

template <Helpers::WrapperType TypeV>
inline
WalkerContext<TypeV>::WalkerContext (Walker<TypeV> const* w,
                                     typename Helpers::Creator<TypeV>::NodeOrDocType nod)
  : walker {w},
    doc {get_doc_from_variant (nod)},
    node {get_node_from_variant (nod)}
{}

template <Helpers::WrapperType TypeV>
inline void
WalkerContext<TypeV>::walk ()
{
  node.traverse (PugiWalker {this});
}

template <Helpers::WrapperType TypeV>
/* static */ inline pugi::xml_document*
WalkerContext<TypeV>::get_doc_from_variant (typename Helpers::Creator<TypeV>::NodeOrDocType node_or_doc)
{
  return boost::apply_visitor (NodeOrDocForDocVisitor{}, node_or_doc);
}

template <Helpers::WrapperType TypeV>
/* static */ inline pugi::xml_node
WalkerContext<TypeV>::get_node_from_variant (typename Helpers::Creator<TypeV>::NodeOrDocType node_or_doc)
{
  return boost::apply_visitor (NodeOrDocForNodeVisitor {}, node_or_doc);
}

template <Helpers::WrapperType TypeV>
WalkerContext<TypeV>::PugiWalker::PugiWalker (WalkerContext<TypeV> const* c,
                                              pugi::xml_document* d)
  : ctx {c},
    doc {d},
    nodes {},
    visited_doc {false},
    doc_depth {0}
{}

template <Helpers::WrapperType TypeV>
bool
WalkerContext<TypeV>::PugiWalker::for_each (pugi::xml_node& node)
{
  if (!run_postprocessing ())
  {
    return false;
  }

  auto const d = depth ();

  switch (node.type ())
  {
  case pugi::node_pcdata:
  case pugi::node_cdata:
    {
      auto t = Helpers::Creator<TypeV>::create_text (TextImpl {this->doc, node});

      return ctx->walker->text (t, d);
    }

  case pugi::node_element:
    {
      auto n = Helpers::Creator<TypeV>::create_node (NodeImpl {doc, node});

      nodes.push (std::make_pair (n, d));
      return ctx->walker->node (n, d);
    }

  case pugi::node_document:
    {
      auto dd = Helpers::Creator<TypeV>::create_doc (DocumentImpl {doc});

      this->visited_doc = true;
      this->doc_depth = d;
      return ctx->walker->doc (dd, d);
    }

  default:
    return true;
  }
}

template <Helpers::WrapperType TypeV>
bool
WalkerContext<TypeV>::PugiWalker::end (pugi::xml_node&)
{
  if (!run_postprocessing ())
  {
    return false;
  }
  if (visited_doc)
  {
    auto d = Helpers::Creator<TypeV>::create_doc (DocumentImpl {doc});
    return ctx->walker->postprocess_doc (d, doc_depth);
  }
  return true;
}

template <Helpers::WrapperType TypeV>
bool
WalkerContext<TypeV>::PugiWalker::run_postprocessing ()
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
    if (!ctx->walker->postprocess_node (p.first, p.second))
    {
      return false;
    }
  }
  return true;
}

template <Helpers::WrapperType TypeV>
inline
WalkerContext<TypeV>::NodeOrDocForDocVisitor::NodeOrDocForDocVisitor () = default;

template <Helpers::WrapperType TypeV>
template <typename NodeOrDocP>
inline pugi::xml_document*
WalkerContext<TypeV>::NodeOrDocForDocVisitor::operator() (NodeOrDocP const& node_or_doc) const
{
  return node_or_doc->as_basic_node ()->impl.doc_ptr;
}

template <Helpers::WrapperType TypeV>
inline
WalkerContext<TypeV>::NodeOrDocForNodeVisitor::NodeOrDocForNodeVisitor () = default;

template <Helpers::WrapperType TypeV>
template <typename NodeOrDocP>
inline pugi::xml_node
WalkerContext<TypeV>::NodeOrDocForNodeVisitor::operator() (NodeOrDocP const& node_or_doc) const
{
  return boost::apply_visitor (BasicNodeImplNodeVisitor {},
                               node_or_doc->as_basic_node ()->impl.doc_or_node);
}

} // namespace PugiXmlDetails

// basic node methods

template <>
inline Type::Wrapper<Document>
BasicNode::document ()
{
  return Document::create (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::Wrapper<Document const>
BasicNode::document () const
{
  return Document::create_const (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline Type::Optional<Type::Wrapper<Node>>
BasicNode::child (Type::StringView name)
{
  auto child_impl = PugiXmlDetails::get_child (impl.doc_ptr, impl.get_node (), name);

  if (child_impl)
  {
    return Type::Optional<Type::Wrapper<Node>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, *child_impl};
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
    return Type::Optional<Type::Wrapper<Node const>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, std::move (*child_impl)};
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

/*
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
*/

template <>
inline void
BasicNode::remove (Type::Wrapper<Node> const& node)
{
  impl.get_node ().remove_child (node->impl.node);
}

template <>
inline bool
BasicNode::equal (BasicNode const& other) const
{
  return impl.equal (other.impl);
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
inline void
BasicNode::swap (BasicNode& other) noexcept
{
  impl.swap (other.impl);
}

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

// attribute methods

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
  return Node::create (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.parent});
}

template <>
inline Type::Wrapper<Node const>
Attribute::parent () const
{
  return Node::create_const (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.parent});
}

template <>
inline bool
Attribute::equal (Attribute const& other) const
{
  return impl.equal (other.impl);
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
inline void
Attribute::swap (Attribute& other) noexcept
{
  impl.swap (other.impl);
}

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

// text methods

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
  return Node::create (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.node.parent ()});
}

template <>
inline Type::Wrapper<Node const>
Text::parent () const
{
  return Node::create_const (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.node.parent ()});
}

template <>
inline TextType
Text::type () const
{
  return PugiXmlDetails::pugi_node_type_to_text_type (impl.node.type ());
}

template <>
inline bool
Text::equal (Text const& other) const
{
  return impl.equal (other.impl);
}

template <>
inline
Text::TextTmpl (XmlImpl::TextImpl i)
  : impl {std::move (i)}
{}

template <>
inline
Text::TextTmpl (Text const& other)
  : impl {other.impl}
{}

template <>
inline
Text::TextTmpl (Text&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
inline
Text::~TextTmpl () noexcept
{}

template <>
inline void
Text::swap (Text& other) noexcept
{
  impl.swap (other.impl);
}

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

// node methods

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
  return PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentVariant<Helpers::WrapperType::Mutable>> (impl);
}

template <>
inline NodeOrDocConst
Node::parent () const
{
  return PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentVariant<Helpers::WrapperType::Const>> (impl);
}

template <>
inline Type::Wrapper<BasicNode>
Node::basic_parent ()
{
  return BasicNode::create (PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentBasicImpl> (impl));
}

template <>
inline Type::Wrapper<BasicNode const>
Node::basic_parent () const
{
  return BasicNode::create_const (PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentBasicImpl> (impl));
}

template <>
inline Type::Optional<Type::Wrapper<Node>>
Node::child (Type::StringView name)
{
  auto child_impl = PugiXmlDetails::get_child (impl.doc_ptr, impl.node, name);

  if (child_impl)
  {
    return Type::Optional<Type::Wrapper<Node>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, std::move (*child_impl)};
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
    return Type::Optional<Type::Wrapper<Node const>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, std::move (*child_impl)};
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

/*
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
*/

template <>
inline Type::Optional<Type::Wrapper<Attribute>>
Node::attribute (Type::StringView name)
{
  auto attr_impl = PugiXmlDetails::get_attribute (impl.doc_ptr, impl.node, name);

  if (attr_impl)
  {
    return Type::Optional<Type::Wrapper<Attribute>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, *attr_impl};
  }

  return Type::Optional<Type::Wrapper<Attribute>> {};
}

template <>
inline Type::Optional<Type::Wrapper<Attribute const>>
Node::attribute (Type::StringView name) const
{
  auto attr_impl = PugiXmlDetails::get_attribute (impl.doc_ptr, impl.node, name);

  if (attr_impl)
  {
    return Type::Optional<Type::Wrapper<Attribute const>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, *attr_impl};
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

    oss << "could not add an attribute named \"" << name << "\"";
    throw std::runtime_error {oss.str ()};
  }

  return Attribute::create (PugiXmlDetails::AttributeImpl {impl.doc_ptr, impl.node, std::move (pattr)});
}

template <>
inline Type::Wrapper<Text>
Node::add_text (TextType text_type)
{
  auto pnode = impl.node.append_child (PugiXmlDetails::text_type_to_pugi_node_type (text_type));

  if (!pnode)
  {
    throw std::runtime_error {"could not add a text child"};
  }

  return Text::create (PugiXmlDetails::TextImpl {impl.doc_ptr, std::move (pnode)});
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
  impl.node.remove_attribute (attribute->impl.attribute);
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
Node::NodeTmpl (Node&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
inline
Node::~NodeTmpl () noexcept
{}

template <>
inline void
Node::swap (Node& other) noexcept
{
  impl.swap (other.impl);
}

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

// document methods

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
    return Type::Optional<Type::Wrapper<Node>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, PugiXmlDetails::NodeImpl {impl.doc_ptr, std::move (tag)}};
  }

  return Type::Optional<Type::Wrapper<Node>> {};
}

template <>
inline Type::Optional<Type::Wrapper<Node const>>
Document::root_tag () const
{
  auto tag = impl.doc_ptr->document_element ();
  if (tag)
  {
    return Type::Optional<Type::Wrapper<Node const>> {Type::OptionalInPlace{}, Type::WrapperInPlace {}, PugiXmlDetails::NodeImpl {impl.doc_ptr, std::move (tag)}};
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

      oss << "document already has a root \"" << (*tag)->name () << "\"";
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
Bundle::~BundleTmpl () noexcept
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
Bundle::BundleTmpl (Bundle&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
void
Bundle::swap (Bundle& other) noexcept
{
  impl.swap (other.impl);
}

template <>
inline Bundle&
Bundle::operator= (Bundle&& other) noexcept
{
  Bundle tmp {std::move (other)};

  swap (tmp);
  return *this;
}

template <>
inline Type::Wrapper<Document>
Bundle::document ()
{
  return Document::create (PugiXmlDetails::DocumentImpl {impl.doc.get ()});
}

template <>
inline Type::Wrapper<Document const>
Bundle::document () const
{
  return Document::create_const (PugiXmlDetails::DocumentImpl {impl.doc.get ()});
}

// walker

template <Helpers::WrapperType TypeV>
inline
Walker<TypeV>::WalkerTmpl ()
{}

template <Helpers::WrapperType TypeV>
inline void
Walker<TypeV>::walk (typename Helpers::Creator<TypeV>::NodeOrDocType node_or_doc) const
{
  typename XmlImpl::WalkerImpl<TypeV> context {this, std::move (node_or_doc)};

  context.walk ();
}

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_XML_PUGI_HH
