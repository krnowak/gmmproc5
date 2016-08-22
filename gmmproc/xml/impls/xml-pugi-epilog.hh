#ifndef GMMPROC_XML_XML_PUGI_HH
#define GMMPROC_XML_XML_PUGI_HH

#ifndef GMMPROC_XML_INCLUDING_IMPL

#error "This file should not be included directly"

#endif

#include "../xml.hh"

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

template <Utils::ViewType TypeV>
class NodeTransform
{
public:
  NodeTransform ();
  NodeTransform (pugi::xml_document* d);

  NodeViewChoice<TypeV> operator () (pugi::xml_node const& n) const;

private:
  pugi::xml_document* doc_ptr;
};

class NodePredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

// attribute

template <Utils::ViewType TypeV>
class AttributeTransform
{
public:
  AttributeTransform ();
  AttributeTransform (pugi::xml_document* d,
                      pugi::xml_node const& p);

  AttributeViewChoice<TypeV> operator () (pugi::xml_attribute const& a) const;

private:
  pugi::xml_document* doc_ptr;
  pugi::xml_node parent;
};

// text

template <Utils::ViewType TypeV>
class TextTransform
{
public:
  TextTransform ();
  TextTransform (pugi::xml_document* d);

  TextViewChoice<TypeV> operator () (pugi::xml_node const& n) const;

private:
  pugi::xml_document* doc_ptr;
};

class TextPredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

// node or text

template <Utils::ViewType TypeV>
class NodeOrTextTransform
{
public:
  NodeOrTextTransform ();
  NodeOrTextTransform (pugi::xml_document* d);

  NodeOrTextChoice<TypeV> operator () (pugi::xml_node const& n) const;

private:
  pugi::xml_document* doc_ptr;
};

class NodeOrTextPredicate
{
public:
  bool operator() (pugi::xml_node const& node) const;
};

template <Utils::ViewType TypeV>
class WalkerContext
{
public:
  static void do_walk (WalkerChoice<TypeV>* w,
                       NodeOrDocChoice<TypeV> nod);

private:
  WalkerContext (WalkerChoice<TypeV>* w,
                 NodeOrDocChoice<TypeV> nod);

  void walk ();

  class PugiWalker;
  class NodeOrDocForDocVisitor;
  class NodeOrDocForNodeVisitor;

  static pugi::xml_document* get_doc_from_variant (NodeOrDocChoice<TypeV> node_or_doc);
  static pugi::xml_node get_node_from_variant (NodeOrDocChoice<TypeV> node_or_doc);

  WalkerChoice<TypeV>* walker;
  pugi::xml_document* doc_ptr;
  pugi::xml_node node;
};

template <Utils::ViewType TypeV>
class WalkerContext<TypeV>::PugiWalker : public pugi::xml_tree_walker
{
public:
  PugiWalker (WalkerContext<TypeV> const* h,
              pugi::xml_document* d);

private:
  bool begin (pugi::xml_node& node) override;
  bool for_each (pugi::xml_node& node) override;
  bool end (pugi::xml_node& node) override;

  bool run_postprocessing ();

  WalkerContext<TypeV> const* ctx;
  pugi::xml_document* doc_ptr;
  std::stack<std::pair<NodeViewChoice<TypeV>, int>> nodes;
  bool visited_doc;
};

template <Utils::ViewType TypeV>
class WalkerContext<TypeV>::NodeOrDocForDocVisitor : public boost::static_visitor<pugi::xml_document*>
{
public:
  NodeOrDocForDocVisitor ();

  template <typename NodeOrDocP>
  pugi::xml_document*
  operator() (NodeOrDocP const& node_or_doc) const;
};

template <Utils::ViewType TypeV>
class WalkerContext<TypeV>::NodeOrDocForNodeVisitor : public boost::static_visitor<pugi::xml_node>
{
public:
  NodeOrDocForNodeVisitor ();

  template <typename NodeOrDocP>
  pugi::xml_node
  operator() (NodeOrDocP const& node_or_doc) const;
};

template <typename TransformP, typename IteratorP>
using TransformIterator = boost::transform_iterator<TransformP, IteratorP>;

template <typename TransformP, typename PredicateP>
using NodeTransformFilterIterator = TransformIterator<TransformP,
                                                      boost::filter_iterator<PredicateP,
                                                                             pugi::xml_node_iterator>>;

template <Utils::ViewType TypeV>
using NodeIteratorForImpl = NodeTransformFilterIterator<NodeTransform<TypeV>,
                                                        NodePredicate>;

template <Utils::ViewType TypeV>
using AttributeIteratorForImpl = TransformIterator<AttributeTransform<TypeV>, pugi::xml_attribute_iterator>;

template <Utils::ViewType TypeV>
using TextIteratorForImpl = NodeTransformFilterIterator<TextTransform<TypeV>, TextPredicate>;

template <Utils::ViewType TypeV>
using NodeOrTextIteratorForImpl = NodeTransformFilterIterator<NodeOrTextTransform<TypeV>, NodeOrTextPredicate>;

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

  template <Utils::ViewType TypeV>
  using NodeIterator = PugiXmlDetails::NodeIteratorForImpl<TypeV>;
  template <Utils::ViewType TypeV>
  using AttributeIterator = PugiXmlDetails::AttributeIteratorForImpl<TypeV>;
  template <Utils::ViewType TypeV>
  using TextIterator = PugiXmlDetails::TextIteratorForImpl<TypeV>;
  template <Utils::ViewType TypeV>
  using NodeOrTextIterator = PugiXmlDetails::NodeOrTextIteratorForImpl<TypeV>;

  template <Utils::ViewType TypeV>
  using NodeRange = boost::iterator_range<NodeIterator<TypeV>>;
  template <Utils::ViewType TypeV>
  using AttributeRange = boost::iterator_range<AttributeIterator<TypeV>>;
  template <Utils::ViewType TypeV>
  using TextRange = boost::iterator_range<TextIterator<TypeV>>;
  template <Utils::ViewType TypeV>
  using NodeOrTextRange = boost::iterator_range<NodeOrTextIterator<TypeV>>;

  template <Utils::ViewType TypeV>
  using WalkerImpl = PugiXmlDetails::WalkerContext<TypeV>;
};

// creators

template <>
template <Utils::ViewType TypeV>
/* static */ inline BasicNodeViewChoice<TypeV>
BasicNode::create (XmlImpl::BasicNodeImpl i)
{
  return BasicNodeViewChoice<TypeV> {Utils::view_in_place, std::move (i)};
}

template <>
template <Utils::ViewType TypeV>
/* static */ inline AttributeViewChoice<TypeV>
Attribute::create (XmlImpl::AttributeImpl i)
{
  return AttributeViewChoice<TypeV> {Utils::view_in_place, std::move (i)};
}

template <>
template <Utils::ViewType TypeV>
/* static */ inline TextViewChoice<TypeV>
Text::create (XmlImpl::TextImpl i)
{
  return TextViewChoice<TypeV> {Utils::view_in_place, std::move (i)};
}

template <>
template <Utils::ViewType TypeV>
/* static */ inline NodeViewChoice<TypeV>
Node::create (XmlImpl::NodeImpl i)
{
  return NodeViewChoice<TypeV> {Utils::view_in_place, std::move (i)};
}

template <>
template <Utils::ViewType TypeV>
/* static */ inline DocumentViewChoice<TypeV>
Document::create (XmlImpl::DocumentImpl i)
{
  return DocumentViewChoice<TypeV> {Utils::view_in_place, std::move (i)};
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
DocumentImpl::~DocumentImpl () noexcept = default;

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
BundleImpl::~BundleImpl () noexcept = default;

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

inline pugi::xml_node
add_child (pugi::xml_node& node,
           Type::StringView name)
{
  auto pnode = node.append_child (name.to_string ().c_str ());

  if (!pnode)
  {
    std::ostringstream oss;

    oss << "could not add a child named \""
        << name
        << "\"";
    throw XmlRuntimeError {oss.str ()};
  }

  return pnode;
}

inline NodeView
insert_child_at_raw (pugi::xml_document* doc_ptr,
                     pugi::xml_node& parent,
                     pugi::xml_node_iterator pos,
                     Type::StringView name)
{
  auto node = [&]() -> auto
    {
      if (pos == parent.end ())
      {
        if (pos == parent.begin ())
        {
          return parent.prepend_child (name.to_string ().c_str ());
        }
        --pos;
        return parent.insert_child_after (name.to_string ().c_str (), *pos);
      }

      return parent.insert_child_before (name.to_string ().c_str (), *pos);
    }();

  return Node::create<Utils::ViewType::Mutable> (NodeImpl {doc_ptr, node});
}

template <typename NodeIteratorP>
inline NodeView
insert_child_at (pugi::xml_document* doc_ptr,
                 pugi::xml_node& parent,
                 NodeIteratorP pos,
                 Type::StringView name)
{
  return insert_child_at_raw (doc_ptr, parent, pos.base ().base (), name);
}

inline AttributeView
insert_attribute_at_raw (pugi::xml_document* doc_ptr,
                         pugi::xml_node& parent,
                         pugi::xml_attribute_iterator pos,
                         Type::StringView name)
{
  auto attribute = [&]() -> auto
    {
      if (pos == parent.attributes_end ())
      {
        if (pos == parent.attributes_begin ())
        {
          return parent.prepend_attribute (name.to_string ().c_str ());
        }
        --pos;
        return parent.insert_attribute_after (name.to_string ().c_str (), *pos);
      }

      return parent.insert_attribute_before (name.to_string ().c_str (), *pos);
    }();

  return Attribute::create<Utils::ViewType::Mutable> (AttributeImpl {doc_ptr, parent, attribute});
}

inline pugi::xml_node_type
text_type_to_pugi_node_type (TextType text_type)
{
  switch (text_type)
  {
  case TextType::Parsed:
    return pugi::node_pcdata;

  case TextType::Raw:
    return pugi::node_cdata;
  }

  std::ostringstream oss;

  oss << "expected Gmmproc::Xml::TextType either Gmmproc::Xml::TextType::Parsed or Gmmproc::Xml::TextType::Raw, got some invalid value ("
      << static_cast<std::underlying_type_t<TextType>> (text_type)
      << ")";
  throw Utils::Bug {oss.str ()};
}

template <typename AttributeIteratorP>
inline AttributeView
insert_attribute_at (pugi::xml_document* doc_ptr,
                     pugi::xml_node& parent,
                     AttributeIteratorP pos,
                     Type::StringView name)
{
  return insert_attribute_at_raw (doc_ptr, parent, pos.base (), name);
}

inline TextView
insert_text_at_raw (pugi::xml_document* doc_ptr,
                    pugi::xml_node& parent,
                    pugi::xml_node_iterator pos,
                    TextType text_type)
{
  auto text = [&]() -> auto
    {
      if (pos == parent.end ())
      {
        if (pos == parent.begin ())
        {
          return parent.prepend_child (text_type_to_pugi_node_type (text_type));
        }
        --pos;
        return parent.insert_child_after (text_type_to_pugi_node_type (text_type), *pos);
      }

      return parent.insert_child_before (text_type_to_pugi_node_type (text_type), *pos);
    }();

  return Text::create<Utils::ViewType::Mutable> (TextImpl {doc_ptr, text});
}

template <typename TextIteratorP>
inline TextView
insert_text_at (pugi::xml_document* doc_ptr,
                pugi::xml_node& parent,
                TextIteratorP pos,
                TextType text_type)
{
  return insert_text_at_raw (doc_ptr, parent, pos.base ().base (), text_type);
}

template <Utils::ViewType TypeV>
class ParentVariant
{
public:
  using ReturnType = NodeOrDocChoice<TypeV>;

  static ReturnType document(NodeImpl const& impl)
  {
    return ReturnType {Document::create<TypeV> (DocumentImpl {impl.doc_ptr})};
  }

  static ReturnType node(NodeImpl const& impl, pugi::xml_node&& parent)
  {
    return ReturnType {Node::create<TypeV> (NodeImpl {impl.doc_ptr, std::move (parent)})};
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

inline std::string
pugi_node_type_name (pugi::xml_node_type type)
{
  switch (type)
  {
#define XML2_PUGI_CASE(t) case t: return std::string {#t}
    XML2_PUGI_CASE (pugi::node_document);
    XML2_PUGI_CASE (pugi::node_element);
    XML2_PUGI_CASE (pugi::node_pcdata);
    XML2_PUGI_CASE (pugi::node_cdata);
    XML2_PUGI_CASE (pugi::node_null);
    XML2_PUGI_CASE (pugi::node_comment);
    XML2_PUGI_CASE (pugi::node_pi);
    XML2_PUGI_CASE (pugi::node_declaration);
    XML2_PUGI_CASE (pugi::node_doctype);
#undef XML2_PUGI_CASE
  }

  std::ostringstream oss;

  oss << "unknown ("
      << static_cast<std::underlying_type_t<pugi::xml_node_type>> (type)
      << ")";

  return oss.str ();
}

template <typename ImplDetailsP>
inline typename ImplDetailsP::ReturnType
get_node_parent_tmpl (NodeImpl const& impl)
{
  auto p = impl.node.parent ();
  if (!p)
  {
    throw Utils::Bug {"node has unexpectedly no parent"};
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
    {
      std::ostringstream oss;

      oss << "expected a node of type either pugi::node_document or pugi::node_element, got "
          << pugi_node_type_name (p.type ());
      throw Utils::Bug {oss.str ()};
    }
  }

  std::ostringstream oss;

  oss << "expected either pugi::node_document or pugi::node_element, got unknown or unhandled node type "
      << pugi_node_type_name (p.type ());
  throw Utils::Bug {oss.str ()};
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

template <Utils::ViewType TypeV>
inline
NodeTransform<TypeV>::NodeTransform ()
  : doc_ptr {}
{}

template <Utils::ViewType TypeV>
inline
NodeTransform<TypeV>::NodeTransform (pugi::xml_document* d)
  : doc_ptr {d}
{}

template <Utils::ViewType TypeV>
inline NodeViewChoice<TypeV>
NodeTransform<TypeV>::operator() (pugi::xml_node const& n) const
{
  return Node::create<TypeV> (NodeImpl {doc_ptr, n});
}

inline bool
NodePredicate::operator() (pugi::xml_node const& node) const
{
  return node.type () == pugi::node_element;
}

template <typename Utils::ViewType TypeV>
using ChildrenRangeDetails = PugiXmlNodeBasedRangeDetailsTmpl<PugiGetChildrenRange, NodeTransform<TypeV>, NodePredicate>;

template <Utils::ViewType TypeV>
inline
AttributeTransform<TypeV>::AttributeTransform ()
  : doc_ptr {}
{}

template <Utils::ViewType TypeV>
inline
AttributeTransform<TypeV>::AttributeTransform (pugi::xml_document* d,
                                               pugi::xml_node const& n)
  : doc_ptr {d},
    parent {n}
{}

template <Utils::ViewType TypeV>
inline AttributeViewChoice<TypeV>
AttributeTransform<TypeV>::operator() (pugi::xml_attribute const& a) const
{
  return Attribute::create<TypeV> (AttributeImpl {doc_ptr, parent, a});
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

template <typename Utils::ViewType TypeV>
using AttributesRangeDetails = AttributeRangeDetailsTmpl<AttributeTransform<TypeV>>;

template <Utils::ViewType TypeV>
inline
TextTransform<TypeV>::TextTransform ()
  : doc_ptr {}
{}

template <Utils::ViewType TypeV>
inline
TextTransform<TypeV>::TextTransform (pugi::xml_document* d)
  : doc_ptr {d}
{}

template <Utils::ViewType TypeV>
inline TextViewChoice<TypeV>
TextTransform<TypeV>::operator() (pugi::xml_node const& n) const
{
  return Text::create<TypeV> (TextImpl {doc_ptr, n});
}

inline bool
TextPredicate::operator() (pugi::xml_node const& node) const
{
  auto const type = node.type ();

  return (type == pugi::node_pcdata) || (type == pugi::node_cdata);
}

template <typename Utils::ViewType TypeV>
using TextsRangeDetails = PugiXmlNodeBasedRangeDetailsTmpl<PugiGetChildrenRange, TextTransform<TypeV>, TextPredicate>;

template <Utils::ViewType TypeV>
inline
NodeOrTextTransform<TypeV>::NodeOrTextTransform ()
  : doc_ptr {}
{}

template <Utils::ViewType TypeV>
inline
NodeOrTextTransform<TypeV>::NodeOrTextTransform (pugi::xml_document* d)
  : doc_ptr {d}
{}

template <Utils::ViewType TypeV>
inline NodeOrTextChoice<TypeV>
NodeOrTextTransform<TypeV>::operator() (pugi::xml_node const& n) const
{
  if (NodePredicate {} (n))
  {
    return NodeOrTextChoice<TypeV> {NodeTransform<TypeV> {doc_ptr} (n)};
  }
  if (TextPredicate {} (n))
  {
    return NodeOrTextChoice<TypeV> {TextTransform<TypeV> {doc_ptr} (n)};
  }

  std::ostringstream oss;

  oss << "expected a node of type either pugi::node_element or pugi::node_text, got "
      << pugi_node_type_name (n.type ());
  throw Utils::Bug {oss.str ()};
}

inline bool
NodeOrTextPredicate::operator() (pugi::xml_node const& node) const
{
  return NodePredicate {} (node) || TextPredicate {} (node);
}

template <typename Utils::ViewType TypeV>
using NodesOrTextsRangeDetails = PugiXmlNodeBasedRangeDetailsTmpl<PugiGetChildrenRange, NodeOrTextTransform<TypeV>, NodeOrTextPredicate>;

template <typename RangeDetailsP>
auto
to_range (pugi::xml_document* doc_ptr,
          pugi::xml_node const& node)
{
  auto r = RangeDetailsP::pugi_range (node);
  auto convert = RangeDetailsP::convert (doc_ptr, node);
  auto rb = r.begin ();
  auto re = r.end ();
  auto fb = RangeDetailsP::filter (rb, re);
  auto fe = RangeDetailsP::filter (re, re);
  auto b = boost::make_transform_iterator (fb, convert);
  auto e = boost::make_transform_iterator (fe, convert);

  return boost::make_iterator_range (b, e);
}

inline TextType
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

  std::ostringstream oss;

  oss << "expected a node of type either pugi::node_pcdata or pugi::node_cdata, got "
      << pugi_node_type_name (node_type);
  throw Utils::Bug {oss.str ()};
}

template <Utils::ViewType TypeV>
/* static */ inline void
WalkerContext<TypeV>::do_walk (WalkerChoice<TypeV>* w,
                               NodeOrDocChoice<TypeV> nod)
{
  WalkerContext context {w, std::move (nod)};

  context.walk ();
}

template <Utils::ViewType TypeV>
inline
WalkerContext<TypeV>::WalkerContext (WalkerChoice<TypeV>* w,
                                     NodeOrDocChoice<TypeV> nod)
  : walker {w},
    doc_ptr {get_doc_from_variant (nod)},
    node {get_node_from_variant (nod)}
{}

template <Utils::ViewType TypeV>
inline void
WalkerContext<TypeV>::walk ()
{
  PugiWalker walker {this, doc_ptr};

  node.traverse (walker);
}

template <Utils::ViewType TypeV>
/* static */ inline pugi::xml_document*
WalkerContext<TypeV>::get_doc_from_variant (NodeOrDocChoice<TypeV> node_or_doc)
{
  return boost::apply_visitor (NodeOrDocForDocVisitor{}, node_or_doc);
}

template <Utils::ViewType TypeV>
/* static */ inline pugi::xml_node
WalkerContext<TypeV>::get_node_from_variant (NodeOrDocChoice<TypeV> node_or_doc)
{
  return boost::apply_visitor (NodeOrDocForNodeVisitor {}, node_or_doc);
}

template <Utils::ViewType TypeV>
WalkerContext<TypeV>::PugiWalker::PugiWalker (WalkerContext<TypeV> const* c,
                                              pugi::xml_document* d)
  : ctx {c},
    doc_ptr {d},
    nodes {},
    visited_doc {false}
{}

template <Utils::ViewType TypeV>
bool
WalkerContext<TypeV>::PugiWalker::begin (pugi::xml_node& node)
{
  switch (node.type ())
  {
  case pugi::node_document:
    {
      auto d = Document::create<TypeV> (DocumentImpl {doc_ptr});

      this->visited_doc = true;
      return ctx->walker->doc (d, 0);
    }

  case pugi::node_element:
    {
      auto n = Node::create<TypeV> (NodeImpl {doc_ptr, node});

      nodes.push (std::make_pair (n, 0));
      return ctx->walker->node (n, 0);
    }

  default:
    return true;
  }
}

template <Utils::ViewType TypeV>
bool
WalkerContext<TypeV>::PugiWalker::for_each (pugi::xml_node& node)
{
  if (!run_postprocessing ())
  {
    return false;
  }

  auto const d = depth () + 1;

  switch (node.type ())
  {
  case pugi::node_pcdata:
  case pugi::node_cdata:
    {
      auto t = Text::create<TypeV> (TextImpl {doc_ptr, node});

      return ctx->walker->text (t, d);
    }

  case pugi::node_element:
    {
      auto n = Node::create<TypeV> (NodeImpl {doc_ptr, node});

      nodes.push (std::make_pair (n, d));
      return ctx->walker->node (n, d);
    }

  default:
    return true;
  }
}

template <Utils::ViewType TypeV>
bool
WalkerContext<TypeV>::PugiWalker::end (pugi::xml_node&)
{
  if (!run_postprocessing ())
  {
    return false;
  }
  if (visited_doc)
  {
    auto d = Document::create<TypeV> (DocumentImpl {doc_ptr});
    return ctx->walker->postprocess_doc (d, 0);
  }
  return true;
}

template <Utils::ViewType TypeV>
bool
WalkerContext<TypeV>::PugiWalker::run_postprocessing ()
{
  auto const d = depth () + 1;

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

template <Utils::ViewType TypeV>
inline
WalkerContext<TypeV>::NodeOrDocForDocVisitor::NodeOrDocForDocVisitor () = default;

template <Utils::ViewType TypeV>
template <typename NodeOrDocP>
inline pugi::xml_document*
WalkerContext<TypeV>::NodeOrDocForDocVisitor::operator() (NodeOrDocP const& node_or_doc) const
{
  return node_or_doc->as_basic_node ()->impl.doc_ptr;
}

template <Utils::ViewType TypeV>
inline
WalkerContext<TypeV>::NodeOrDocForNodeVisitor::NodeOrDocForNodeVisitor () = default;

template <Utils::ViewType TypeV>
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
inline DocumentView
BasicNode::document ()
{
  return Document::create<Utils::ViewType::Mutable> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline DocumentConstView
BasicNode::document () const
{
  return Document::create<Utils::ViewType::Const> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline XmlImpl::NodeRange<Utils::ViewType::Mutable>
BasicNode::children ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Utils::ViewType::Mutable>> (impl.doc_ptr, impl.get_node ());
}

template <>
inline XmlImpl::NodeRange<Utils::ViewType::Const>
BasicNode::children () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Utils::ViewType::Const>> (impl.doc_ptr, impl.get_node ());
}

template <>
inline void
BasicNode::remove (NodeView const& node)
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
inline DocumentView
Attribute::document ()
{
  return Document::create<Utils::ViewType::Mutable> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline DocumentConstView
Attribute::document () const
{
  return Document::create<Utils::ViewType::Const> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
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
inline NodeView
Attribute::parent ()
{
  return Node::create<Utils::ViewType::Mutable> (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.parent});
}

template <>
inline NodeConstView
Attribute::parent () const
{
  return Node::create<Utils::ViewType::Const> (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.parent});
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
inline DocumentView
Text::document ()
{
  return Document::create<Utils::ViewType::Mutable> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline DocumentConstView
Text::document () const
{
  return Document::create<Utils::ViewType::Const> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
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
inline NodeView
Text::parent ()
{
  return Node::create<Utils::ViewType::Mutable> (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.node.parent ()});
}

template <>
inline NodeConstView
Text::parent () const
{
  return Node::create<Utils::ViewType::Const> (PugiXmlDetails::NodeImpl {impl.doc_ptr, impl.node.parent ()});
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
inline BasicNodeView
Node::as_basic_node ()
{
  return BasicNode::create<Utils::ViewType::Mutable> (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr, impl.node});
}

template <>
inline BasicNodeConstView
Node::as_basic_node () const
{
  return BasicNode::create<Utils::ViewType::Const> (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr, impl.node});
}

template <>
inline DocumentView
Node::document ()
{
  return Document::create<Utils::ViewType::Mutable> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
}

template <>
inline DocumentConstView
Node::document () const
{
  return Document::create<Utils::ViewType::Const> (PugiXmlDetails::DocumentImpl {impl.doc_ptr});
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
  return PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentVariant<Utils::ViewType::Mutable>> (impl);
}

template <>
inline NodeOrDocConst
Node::parent () const
{
  return PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentVariant<Utils::ViewType::Const>> (impl);
}

template <>
inline BasicNodeView
Node::basic_parent ()
{
  return BasicNode::create<Utils::ViewType::Mutable> (PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentBasicImpl> (impl));
}

template <>
inline BasicNodeConstView
Node::basic_parent () const
{
  return BasicNode::create<Utils::ViewType::Const> (PugiXmlDetails::get_node_parent_tmpl<PugiXmlDetails::ParentBasicImpl> (impl));
}

template <>
inline XmlImpl::NodeRange<Utils::ViewType::Mutable>
Node::children ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Utils::ViewType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::NodeRange<Utils::ViewType::Const>
Node::children () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::ChildrenRangeDetails<Utils::ViewType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::AttributeRange<Utils::ViewType::Mutable>
Node::attributes ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::AttributesRangeDetails<Utils::ViewType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::AttributeRange<Utils::ViewType::Const>
Node::attributes () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::AttributesRangeDetails<Utils::ViewType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::TextRange<Utils::ViewType::Mutable>
Node::texts ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::TextsRangeDetails<Utils::ViewType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::TextRange<Utils::ViewType::Const>
Node::texts () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::TextsRangeDetails<Utils::ViewType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::NodeOrTextRange<Utils::ViewType::Mutable>
Node::all ()
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NodesOrTextsRangeDetails<Utils::ViewType::Mutable>> (impl.doc_ptr, impl.node);
}

template <>
inline XmlImpl::NodeOrTextRange<Utils::ViewType::Const>
Node::all () const
{
  return PugiXmlDetails::to_range<PugiXmlDetails::NodesOrTextsRangeDetails<Utils::ViewType::Const>> (impl.doc_ptr, impl.node);
}

template <>
inline NodeView
Node::insert_child_at (XmlImpl::NodeIterator<Utils::ViewType::Mutable> pos,
                       Type::StringView name)
{
  return PugiXmlDetails::insert_child_at (impl.doc_ptr,
                                          impl.node,
                                          pos,
                                          name);
}

template <>
inline NodeView
Node::insert_child_at (XmlImpl::NodeIterator<Utils::ViewType::Const> pos,
                       Type::StringView name)
{
  return PugiXmlDetails::insert_child_at (impl.doc_ptr,
                                          impl.node,
                                          pos,
                                          name);
}

template <>
inline NodeView
Node::insert_child_at (XmlImpl::TextIterator<Utils::ViewType::Mutable> pos,
                       Type::StringView name)
{
  return PugiXmlDetails::insert_child_at (impl.doc_ptr,
                                          impl.node,
                                          pos,
                                          name);
}

template <>
inline NodeView
Node::insert_child_at (XmlImpl::TextIterator<Utils::ViewType::Const> pos,
                       Type::StringView name)
{
  return PugiXmlDetails::insert_child_at (impl.doc_ptr,
                                          impl.node,
                                          pos,
                                          name);
}

template <>
inline NodeView
Node::insert_child_at (XmlImpl::NodeOrTextIterator<Utils::ViewType::Mutable> pos,
                       Type::StringView name)
{
  return PugiXmlDetails::insert_child_at (impl.doc_ptr,
                                          impl.node,
                                          pos,
                                          name);
}

template <>
inline NodeView
Node::insert_child_at (XmlImpl::NodeOrTextIterator<Utils::ViewType::Const> pos,
                       Type::StringView name)
{
  return PugiXmlDetails::insert_child_at (impl.doc_ptr,
                                          impl.node,
                                          pos,
                                          name);
}

template <>
inline AttributeView
Node::insert_attribute_at (XmlImpl::AttributeIterator<Utils::ViewType::Mutable> pos,
                           Type::StringView name)
{
  return PugiXmlDetails::insert_attribute_at (impl.doc_ptr,
                                              impl.node,
                                              pos,
                                              name);
}

template <>
inline AttributeView
Node::insert_attribute_at (XmlImpl::AttributeIterator<Utils::ViewType::Const> pos,
                           Type::StringView name)
{
  return PugiXmlDetails::insert_attribute_at (impl.doc_ptr,
                                              impl.node,
                                              pos,
                                              name);
}

template <>
inline TextView
Node::insert_text_at (XmlImpl::NodeIterator<Utils::ViewType::Mutable> pos,
                      TextType text_type)
{
  return PugiXmlDetails::insert_text_at (impl.doc_ptr,
                                         impl.node,
                                         pos,
                                         text_type);
}

template <>
inline TextView
Node::insert_text_at (XmlImpl::NodeIterator<Utils::ViewType::Const> pos,
                      TextType text_type)
{
  return PugiXmlDetails::insert_text_at (impl.doc_ptr,
                                         impl.node,
                                         pos,
                                         text_type);
}

template <>
inline TextView
Node::insert_text_at (XmlImpl::TextIterator<Utils::ViewType::Mutable> pos,
                      TextType text_type)
{
  return PugiXmlDetails::insert_text_at (impl.doc_ptr,
                                         impl.node,
                                         pos,
                                         text_type);
}

template <>
inline TextView
Node::insert_text_at (XmlImpl::TextIterator<Utils::ViewType::Const> pos,
                      TextType text_type)
{
  return PugiXmlDetails::insert_text_at (impl.doc_ptr,
                                         impl.node,
                                         pos,
                                         text_type);
}

template <>
inline TextView
Node::insert_text_at (XmlImpl::NodeOrTextIterator<Utils::ViewType::Mutable> pos,
                      TextType text_type)
{
  return PugiXmlDetails::insert_text_at (impl.doc_ptr,
                                         impl.node,
                                         pos,
                                         text_type);
}

template <>
inline TextView
Node::insert_text_at (XmlImpl::NodeOrTextIterator<Utils::ViewType::Const> pos,
                      TextType text_type)
{
  return PugiXmlDetails::insert_text_at (impl.doc_ptr,
                                         impl.node,
                                         pos,
                                         text_type);
}

template <>
inline void
Node::remove (NodeView const& node)
{
  impl.node.remove_child (node->impl.node);
}

template <>
inline void
Node::remove (AttributeView const& attribute)
{
  impl.node.remove_attribute (attribute->impl.attribute);
}

template <>
inline void
Node::remove (TextView const& text)
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
inline BasicNodeView
Document::as_basic_node ()
{
  return BasicNode::create<Utils::ViewType::Mutable> (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr});
}

template <>
inline BasicNodeConstView
Document::as_basic_node () const
{
  return BasicNode::create<Utils::ViewType::Const> (PugiXmlDetails::BasicNodeImpl {impl.doc_ptr});
}

template <>
inline Type::Optional<NodeView>
Document::root_tag ()
{
  auto tag = impl.doc_ptr->document_element ();
  if (tag)
  {
    return Type::Optional<NodeView> {Type::OptionalInPlace {}, Utils::view_in_place, PugiXmlDetails::NodeImpl {impl.doc_ptr, std::move (tag)}};
  }

  return Type::Optional<NodeView> {};
}

template <>
inline Type::Optional<NodeConstView>
Document::root_tag () const
{
  auto tag = impl.doc_ptr->document_element ();
  if (tag)
  {
    return Type::Optional<NodeConstView> {Type::OptionalInPlace {}, Utils::view_in_place, PugiXmlDetails::NodeImpl {impl.doc_ptr, std::move (tag)}};
  }

  return Type::Optional<NodeConstView> {};
}

template <>
inline NodeView
Document::add_root (Type::StringView name)
{
  {
    auto tag = root_tag ();
    if (tag)
    {
      std::ostringstream oss;

      oss << "document already has a root named \""
          << (*tag)->name ()
          << "\"";
      throw XmlRuntimeError (oss.str ());
    }
  }

  return Node::create<Utils::ViewType::Mutable> (PugiXmlDetails::NodeImpl {impl.doc_ptr, PugiXmlDetails::add_child (*impl.doc_ptr, name)});
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
Document::~DocumentTmpl () noexcept
{}

template <>
inline void
Document::swap (Document& other) noexcept
{
  impl.swap (other.impl);
}

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
    ParseError ex {result.description ()};

    ex.set_extra_data (result);
    throw ex;
  }
}

template <>
inline
Bundle::BundleTmpl (Bundle&& other) noexcept
  : impl {std::move (other.impl)}
{}

template <>
inline void
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
inline DocumentView
Bundle::document ()
{
  return Document::create<Utils::ViewType::Mutable> (PugiXmlDetails::DocumentImpl {impl.doc.get ()});
}

template <>
inline DocumentConstView
Bundle::document () const
{
  return Document::create<Utils::ViewType::Const> (PugiXmlDetails::DocumentImpl {impl.doc.get ()});
}

// walker

template <>
inline
Walker::WalkerTmpl ()
{}

template <>
inline void
Walker::walk (NodeOrDoc node_or_doc)
{
  XmlImpl::WalkerImpl<Utils::ViewType::Mutable>::do_walk (this, std::move (node_or_doc));
}

template <>
inline
ConstWalker::WalkerTmpl ()
{}

template <>
inline void
ConstWalker::walk (NodeOrDocConst node_or_doc)
{
  XmlImpl::WalkerImpl<Utils::ViewType::Const>::do_walk (this, std::move (node_or_doc));
}

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_XML_PUGI_HH
