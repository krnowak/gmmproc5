#ifndef GMMPROC_XML_XML_PUGI_HH
#define GMMPROC_XML_XML_PUGI_HH
#ifndef GMMPROC_XML_INCLUDING_IMPL

#error "This file should not be included directly"

#endif

#define BOOST_RESULT_OF_USE_DECLTYPE

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

class SwappableXmlNode : public pugi::xml_node
{
public:
  using pugi::xml_node::xml_node;

  SwappableXmlNode (pugi::xml_node const& n);

  void swap (SwappableXmlNode& node);
};

class BasicNodeWrapper
{
public:
  BasicNodeWrapper (Type::Ref<Document> d,
                    SwappableXmlNode n);
  BasicNodeWrapper (Type::Ref<Document> d,
                    Type::Ref<pugi::xml_document> d_ref);

  pugi::xml_node&
  get_node ();

  void
  swap (BasicNodeWrapper&& wrapper);

  Type::Ref<Document> doc;
  Type::Variant<SwappableXmlNode, Type::Ref<pugi::xml_document>> node;
};

class NodeWrapper
{
public:
  NodeWrapper (Type::Ref<Document> d,
               SwappableXmlNode n);

  BasicNode basic_node;
  SwappableXmlNode node;
};

class NodeTransform
{
public:
  NodeTransform ();
  NodeTransform (Type::Ref<Document> d);

  Node operator () (pugi::xml_node const& n) const;

private:
  Document *doc;
};

class TextWrapper
{
public:
  TextWrapper (Type::Ref<Document> d,
               pugi::xml_node n);

  Type::Ref<Document> doc;
  pugi::xml_node node;
};

class TextTransform
{
public:
  TextTransform ();
  TextTransform (Type::Ref<Document> d);

  Text operator () (pugi::xml_node const& n) const;

private:
  Document *doc;
};

class AttributeWrapper
{
public:
  AttributeWrapper (Type::Ref<Document> d,
                    pugi::xml_node p,
                    pugi::xml_attribute a);

  Type::Ref<Document> doc;
  pugi::xml_node parent;
  pugi::xml_attribute attr;
};

class AttributeTransform
{
public:
  AttributeTransform ();
  AttributeTransform (Type::Ref<Document> d,
                      pugi::xml_node p);

  Attribute operator () (pugi::xml_attribute const& a) const;

private:
  Document *doc;
  pugi::xml_node parent;
};

class NodeOrTextTransform
{
public:
  NodeOrTextTransform ();
  NodeOrTextTransform (Type::Ref<Document> d);

  NodeOrText operator () (pugi::xml_node const& n) const;

private:
  Document *doc;
};

class NodePredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

class TextPredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

class NodeOrTextPredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

using NodeFilterIterator = boost::filter_iterator<NodePredicate, pugi::xml_node_iterator>;
using NodeTransformIterator = boost::transform_iterator<NodeTransform, NodeFilterIterator>;

using AttributeIterator = boost::transform_iterator<AttributeTransform, pugi::xml_attribute_iterator>;

using TextFilterIterator = boost::filter_iterator<TextPredicate, pugi::xml_node_iterator>;
using TextTransformIterator = boost::transform_iterator<TextTransform, TextFilterIterator>;

using NodeOrTextFilterIterator = boost::filter_iterator<NodeOrTextPredicate, pugi::xml_node_iterator>;
using NodeOrTextTransformIterator = boost::transform_iterator<NodeOrTextTransform, NodeOrTextFilterIterator>;

class DocumentWrapper
{
public:
  DocumentWrapper ();
  DocumentWrapper (DocumentWrapper const&) = delete;
  DocumentWrapper (DocumentWrapper&& w);

  void swap (DocumentWrapper&& w);

  std::unique_ptr<pugi::xml_document> ptr;
  BasicNode basic_node;
};

class WalkerWrapper final : public pugi::xml_tree_walker
{
public:
  WalkerWrapper (Walker& w);

  void set_doc (Type::Ref<Document> d);

private:
  virtual bool
  for_each (pugi::xml_node& node) override;

  virtual bool
  end (pugi::xml_node& node) override;

  bool
  run_postprocessing ();

  Walker& walker;
  Document* doc;
  std::stack<std::pair<Node, int>> nodes;
};

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
text_type_to_pugi_node_type (pugi::xml_node_type node_type)
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

class XmlImpl
{
public:
  using BasicNodeImpl = PugiXmlDetails::BasicNodeWrapper;
  using NodeImpl = PugiXmlDetails::NodeWrapper;
  using TextImpl = PugiXmlDetails::TextWrapper;
  using AttributeImpl = PugiXmlDetails::AttributeWrapper;
  using DocumentImpl = PugiXmlDetails::DocumentWrapper;
  using WalkerImpl = PugiXmlDetails::WalkerWrapper;

  using NodeRange = boost::iterator_range<PugiXmlDetails::NodeTransformIterator>;
  using AttributeRange = boost::iterator_range<PugiXmlDetails::AttributeIterator>;
  using TextRange = boost::iterator_range<PugiXmlDetails::TextTransformIterator>;
  using NodeOrTextRange = boost::iterator_range<PugiXmlDetails::NodeOrTextTransformIterator>;
};

namespace PugiXmlDetails
{

// swappable xml node

SwappableXmlNode::SwappableXmlNode (pugi::xml_node const& n)
  : pugi::xml_node {n.internal_object ()}
{}

void
SwappableXmlNode::swap (SwappableXmlNode& node)
{
  using std::swap;

  swap (_root, node._root);
}

// basic node wrapper

BasicNodeWrapper::BasicNodeWrapper (Type::Ref<Document> d,
                                    SwappableXmlNode n)
  : doc {d},
    node {std::move (n)}
{}

BasicNodeWrapper::BasicNodeWrapper (Type::Ref<Document> d,
                                    Type::Ref<pugi::xml_document> d_ref)
  : doc {d},
    node {d_ref}
{}

class BasicNodeWrapperNodeVisitor : public boost::static_visitor<pugi::xml_node&>
{
public:
  pugi::xml_node&
  operator() (SwappableXmlNode& node)
  {
    return node;
  }

  pugi::xml_node&
  operator() (Type::Ref<pugi::xml_document>& d_ref)
  {
    return d_ref.get ();
  }
};

pugi::xml_node&
BasicNodeWrapper::get_node ()
{
  return boost::apply_visitor (BasicNodeWrapperNodeVisitor {}, node);
}

void
BasicNodeWrapper::swap (BasicNodeWrapper&& wrapper)
{
  using std::swap;

  swap (doc, std::move (wrapper.doc));
  swap (node, std::move (wrapper.node));
}

// node wrapper

inline
NodeWrapper::NodeWrapper (Type::Ref<Document> d,
                          SwappableXmlNode n)
  : basic_node {std::move (d), n},
    node {n}
{}

inline
NodeTransform::NodeTransform ()
  : doc {}
{}

inline
NodeTransform::NodeTransform (Type::Ref<Document> d)
  : doc {std::addressof (d.get ())}
{}

inline Node
NodeTransform::operator() (pugi::xml_node const& n) const
{
  return Node {NodeWrapper {std::ref (*doc), SwappableXmlNode {n}}};
}

inline
TextWrapper::TextWrapper (Type::Ref<Document> d,
                          pugi::xml_node n)
  : doc {std::move (d)},
    node {std::move (n)}
{}

inline
TextTransform::TextTransform ()
  : doc {}
{}

inline
TextTransform::TextTransform (Type::Ref<Document> d)
  : doc {std::addressof (d.get ())}
{}

inline Text
TextTransform::operator() (pugi::xml_node const& n) const
{
  return Text {TextWrapper {std::ref (*doc), n}};
}

inline
AttributeWrapper::AttributeWrapper (Type::Ref<Document> d,
                                    pugi::xml_node p,
                                    pugi::xml_attribute a)
  : doc {std::move (d)},
    parent {std::move (p)},
    attr {std::move (a)}
{}

inline
AttributeTransform::AttributeTransform ()
  : doc {},
    parent {}
{}

inline
AttributeTransform::AttributeTransform (Type::Ref<Document> d,
                                        pugi::xml_node p)
  : doc {std::addressof (d.get ())},
    parent {std::move (p)}
{}

inline Attribute
AttributeTransform::operator() (pugi::xml_attribute const& a) const
{
  return Attribute {AttributeWrapper {std::ref (*doc), parent, a}};
}

inline
NodeOrTextTransform::NodeOrTextTransform ()
  : doc {}
{}

inline
NodeOrTextTransform::NodeOrTextTransform (Type::Ref<Document> d)
  : doc {std::addressof (d.get ())}
{}

inline NodeOrText
NodeOrTextTransform::operator() (pugi::xml_node const& node)
{
  if (TextPredicate {} (node))
  {
    return NodeOrText {Node {NodeWrapper {std::ref (doc), node}}};
  }

  if (NodePredicate {} (node))
  {
    return NodeOrText {Text {node}};
  }

  throw std::runtime_error("Argh");
}

inline bool
NodePredicate::operator() (pugi::xml_node const& node) const
{
  return node.type () == pugi::node_element;
}

inline bool
TextPredicate::operator() (pugi::xml_node const& node) const
{
  auto const type = node.type ();

  return (type == pugi::node_pcdata) || (type == pugi::node_cdata);
}

inline bool
NodeOrTextPredicate::operator() (pugi::xml_node const& node) const
{
   return NodePredicate {} (node) || TextPredicate {} (node);
}

DocumentWrapper::DocumentWrapper ()
  : ptr {std::make_unique<pugi::xml_document> ()},
    base {BasicNodeWrapper {*ptr.get ()}}
{}

DocumentWrapper::DocumentWrapper (DocumentWrapper&& w)
  : ptr {std::move (w.ptr)},
    base {std::move (w.base)}
{}

void
DocumentWrapper::swap (DocumentWrapper&& w)
  : ptr {std::make_unique<pugi::xml_document> ()},
    base {BasicNodeWrapper {ptr.get ()}}
{
  ptr.swap (std::move (w.ptr));
  base.swap (std::move (w.base));
}

inline
WalkerWrapper::WalkerWrapper (Walker& w)
  : walker {w},
    doc {},
    nodes {}
{}

inline void
WalkerWrapper::set_doc (Type::Ref<Document> d)
{
  doc = std::addressof (d.get ());
}

bool
WalkerWrapper::for_each (pugi::xml_node& node)
{
  auto const d = depth ();

  switch (node.type ())
  {
  case pugi::node_pcdata:
  case pugi::node_cdata:
    {
      Text t {node};

      return walker.text (t, d);
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
WalkerWrapper::end (pugi::xml_node&)
{
  auto result = run_postprocessing ();
  doc = nullptr;
  return result;
}

bool
WalkerWrapper::run_postprocessing ()
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

template <typename PugiChildrenRange>
XmlImpl::NodeRange
to_node_range (Type::Ref<Document> const& doc,
               PugiChildrenRange& range) const
{
  auto convert = PugiXmlDetails::NodeTransform {impl.doc};
  auto rb = range.begin ();
  auto re = range.end ();
  auto fb = PugiXmlDetails::NodeFilterIterator {PugiXmlDetails::NodePredicate {}, rb, re};
  auto fe = PugiXmlDetails::NodeFilterIterator {PugiXmlDetails::NodePredicate {}, re, re};
  auto b = PugiXmlDetails::NodeTransformIterator {fb, convert};
  auto e = PugiXmlDetails::NodeTransformIterator {fe, convert};
  return XmlImpl::NodeRange {b, e};
}

Type::Optional<Node>
get_child (Type::Ref<Document> doc,
           pugi::xml_node& node,
           Type::StringView name)
{
  auto c = node.child (name.to_string ().c_str ());

  if (c)
  {
    NodeWrapper wrapper {doc, std::move (c)};

    return Type::Optional<Node> {Type::OptionalInPlace{}, std::move (wrapper)};
  }

  return Type::Optional<Node> {};
}

} // namespace PugiXmlDetails

// basic node

template <>
inline
BasicNode::~BasicNodeTmpl() = default;

template <>
inline Document const&
BasicNode::document () const
{
  return impl.doc.get ();
}

template <>
inline Document&
BasicNode::document ()
{
  return impl.doc.get ();
}

template <>
inline Type::Optional<Node>
BasicNode::child (Type::StringView name) const
{
  return PugiXmlDetails::get_child (impl.doc, impl.get_node (), name);
}

template <>
inline XmlImpl::NodeRange
BasicNode::children () const
{
  return PugiXmlDetails::to_node_range (impl.doc,
                                        impl.get_node ().children ());
}

template <>
inline XmlImpl::NodeRange
BasicNode::children (Type::StringView name) const
{
  return PugiXmlDetails::to_node_range (impl.doc,
                                        impl.get_node ().children (name.to_string ().c_str ()));
}

template <>
inline void
BasicNode::remove (Node const& child)
{
  impl.get_node ().remove_child (child.impl.node);
}

template <>
inline bool
BasicNode::equal (BasicNode const& rhs) const
{
  return impl.get_node() == rhs.impl.get_node ();
}

template <>
inline void
BasicNode::swap (BasicNode& base)
{
  impl.swap (base.impl);
}

// node methods

template <>
inline
Node::~NodeTmpl () = default;

template <>
Node::NodeTmpl (NodeTmpl const& node) = default;

template <>
Node::NodeTmpl (NodeTmpl&& node) = default;

template <>
inline
Node&
Node::operator= (NodeTmpl const& node) = default;

template <>
inline
Node&
Node::operator= (NodeTmpl&& node) = default;

template <>
inline
void
Node::swap (NodeTmpl& node)
{
  impl.basic_node.swap (node.impl.basic_node);
  impl.node.swap (node.impl.node);
}

template <>
inline BasicNode const&
Node::as_basic_node () const
{
  return impl.basic_node;
}

template <>
inline BasicNode&
Node::as_basic_node ()
{
  return impl.basic_node;
}

template <>
inline Document const&
Node::document () const
{
  return impl.basic_node.impl.doc.get ();
}

template <>
inline Document&
Node::document ()
{
  return impl.basic_node.impl.doc.get ();
}

template <>
inline Type::StringView
Node::name () const
{
  return impl.node.name ();
}

template <>
NodeOrDoc
Node::parent () const
{
  auto p = impl.node.parent ();
  if (!p)
  {
    throw std::runtime_error ("arf");
  }

  switch (p.type ())
  {
  case pugi::node_document:
    return NodeOrDoc {impl.basic_node.impl.doc};

  case pugi::node_element:
    return NodeOrDoc {Node {PugiXmlDetails::BasicNodeWrapper {impl.basic_node.impl.doc, std::move (p)}}};

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

template <>
BasicNode
Node::basic_parent() const
{
  auto p = SwappableXmlNode {impl.node.parent ()};
  if (!p)
  {
    throw std::runtime_error ("arg");
  }

  switch (p.type ())
  {
  case pugi::node_document:
    return BasicNode {PugiXmlDetails::BasicNodeWrapper {impl.basic_node.impl.doc, std::ref (*impl.basic_node.impl.doc.get ().impl.ptr.get ())}};

  case pugi::node_element:
    return BasicNode {PugiXmlDetails::BasicNodeWrapper {impl.basic_node.impl.doc, std::move (p)}};

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

template <>
inline Type::Optional<Node>
Node::child (Type::StringView name) const
{
  return PugiXmlDetails::get_child (impl.basic_node.impl.doc,
                                    impl.node,
                                    name);
}

template <>
inline XmlImpl::NodeRange
Node::children () const
{
  return PugiXmlDetails::to_node_range (impl.basic_node.impl.doc,
                                        impl.node.children ());
}

template <>
inline XmlImpl::NodeRange
Node::children (Type::StringView name) const
{
  return PugiXmlDetails::to_node_range (impl.basic_node.impl.doc,
                                        impl.node.children (name.to_string ().c_str ()));
}

template <>
inline Type::Optional<Attribute>
Node::attribute (Type::StringView name) const
{
  auto a = impl.node.attribute (name.to_string ().c_str ());
  if (a)
  {
    AttributeWrapper wrapper {impl.basic_node.impl.doc, impl.node, std::move (a)};

    return Type::Optional<Attribute> {Type::OptionalInPlace{}, std::move (wrapper)};
  }

  return Type::Optional<Attribute> {};
}

template <>
inline XmlImpl::AttributeRange
Node::attributes () const
{
  auto r = impl.attributes ();
  auto convert = PugiXmlDetails::AttributeTransform {impl.basic_node.impl.doc, impl.node};
  auto b = XmlImpl::AttributeRange::iterator {r.begin (), convert};
  auto e = XmlImpl::AttributeRange::iterator {r.end (), convert};
  return XmlImpl::AttributeRange {b, e};
}

template <>
inline XmlImpl::TextRange
Node::texts () const
{
  auto r = impl.children ();
  auto convert = PugiXmlDetails::TextTransform {impl.basic_node.impl.doc};
  auto rb = r.begin ();
  auto re = r.end ();
  auto fb = PugiXmlDetails::TextFilterIterator {PugiXmlDetails::TextPredicate {}, rb, re};
  auto fe = PugiXmlDetails::TextFilterIterator {PugiXmlDetails::TextPredicate {}, re, re};
  auto b = PugiXmlDetails::TextTransformIterator {fb, convert};
  auto e = PugiXmlDetails::TextTransformIterator {fe, convert};
  return XmlImpl::TextRange {b, e};
}

template <>
inline XmlImpl::NodeOrTextRange
Node::all () const
{
  auto r = impl.children ();
  auto convert = PugiXmlDetails::NodeOrTextTransform {impl.basic_node.impl.doc};
  auto rb = r.begin ();
  auto re = r.end ();
  auto fb = PugiXmlDetails::NodeOrTextFilterIterator {PugiXmlDetails::NodeOrTextPredicate {}, rb, re};
  auto fe = PugiXmlDetails::NodeOrTextFilterIterator {PugiXmlDetails::NodeOrTextPredicate {}, re, re};
  auto b = PugiXmlDetails::NodeOrTextTransformIterator {fb, convert};
  auto e = PugiXmlDetails::NodeOrTextTransformIterator {fe, convert};
  return XmlImpl::NodeOrTextRange {b, e};
}

template <>
inline Node
Node::add_child (Type::StringView name)
{
  auto pnode = PugiXmlDetails::SwappableXmlNode {impl.node.append_child ()};

  if (!pnode)
  {
    std::ostringstream oss;

    oss << "could not add a child named \"" << name << "\"";
    throw std::runtime_error {oss.str ()};
  }
  pnode.set_name (name.to_string ().c_str ());
  return Node {PugiXmlDetails::NodeWrapper {impl.basic_node.impl.doc, pnode}};
}

template <>
inline Attribute
Node::add_attribute (Type::StringView name,
                     Type::StringView value)
{
  auto pattr = impl.node.append_attribute (name.to_string ().c_str ());

  if (!pattr)
  {
    std::ostringstream oss;

    oss << "could not add an attribute named \"" << name << "\" with value \"" << value << "\"";
    throw std::runtime_error {oss.str ()};
  }
  pattr.set_value (value.to_string ().c_str ());
  return Attribute {PugiXmlDetails::AttributeWrapper {impl.doc, impl.node, std::move (pattr)}};
}

template <>
inline Text
Node::add_text (Type::StringView value,
                TextType text_type)
{
  auto pnode = impl.append_child (PugiXmlDetails::text_type_to_pugi_node_type (text_type));

  if (!pnode)
  {
    throw std::runtime_error {"could not add a text child"};
  }
  pnode.set_value (value.to_string ().c_str ());
  return Text {TextWrapper {impl.doc, pnode}};
}

template <>
inline void
Node::remove (Node const& child)
{
  impl.remove_child (child.impl);
}

template <>
inline void
Node::remove (Attribute const &attribute)
{
  impl.remove_attribute (attribute.impl.attr);
}

template <>
inline void
Node::remove (Text const& text)
{
  impl.remove_child (text.impl);
}

// text methods

template <>
inline Type::StringView
Text::text () const
{
  return impl.value ();
}

template <>
inline Node
Text::parent () const
{
  return Node {PugiXmlDetails::NodeWrapper {impl.doc, impl.node.parent ()}};
}

template <>
inline TextType
Text::type () const
{
  return PugiXmlDetails::text_type_to_pugi_node_type (impl.type ());
}

template <>
inline bool
Text::equal (Text const& rhs)
{
  return impl == rhs.impl;
}

// attribute methods

template <>
inline Type::StringView
Attribute::name () const
{
  return impl.attr.name ();
}

template <>
inline Type::StringView
Attribute::value () const
{
  return impl.attr.value ();
}

template <>
inline Node
Attribute::parent () const
{
  return Node {NodeWrapper {impl.doc, impl.parent}};
}

template <>
inline bool
Attribute::equal (Attribute const& rhs)
{
  return impl.attr == rhs.impl.attr;
}

// walker methods

template <>
inline
Walker::WalkerTmpl ()
  : impl {*this}
{}

template <>
inline void
Walker::walk (Document& doc)
{
  impl.set_doc (std::ref (doc));
  doc.impl->traverse (impl);
}

// document methods

template <>
inline
Document::DocumentTmpl ()
  : impl {}
{}

template <>
inline
Document::DocumentTmpl (Type::StringView filename)
  : Document {}
{
  auto result = impl.ptr->load_file (filename.to_string ().c_str ());

  if (!result)
  {
    std::ostringstream oss;

    oss << "Failed to parse '" << filename << "': " << result.description ();
    throw ParseError {oss.str ()};
  }
}

template <>
inline
Document::DocumentTmpl (Document&& doc)
  : impl {std::move (doc.impl)}
{}

template <>
inline Document&
Document::operator= (Document&& doc)
{
  DocumentWrapper tmp {std::move (doc.impl)};

  impl.swap (tmp);
  return *this;
}

template <>
BasicNode
Document::as_basic_node () const
{
  return BasicNode {BasicNodeWrapper {std::ref (*this), impl.get ()}};
}

template <>
inline Type::Optional<Node>
Document::root_tag () const
{
  auto tag = impl->document_element ();
  if (tag)
  {
    return Type::Optional<Node> {Type::OptionalInPlace{}, std::move (tag)};
  }

  return Type::Optional<Node> {};
}

template <>
inline Node
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
  return Node {impl->root ()}.add_child (name);
}

template <>
inline void
Document::save (std::ostream& os)
{
  impl->save (os, "  ");
}

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_XML_PUGI_HH
