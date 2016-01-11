#ifndef MM_XML_BASE_XML_PUGI_HH
#define MM_XML_BASE_XML_PUGI_HH

#ifndef MM_XML_BASE_INCLUDING_IMPL

#error This file should not be included directly

#endif

#define BOOST_RESULT_OF_USE_DECLTYPE

#include "xml.hh"

#include <pugixml.hpp>

#include <boost/range/iterator_range_core.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <experimental/optional>

#include <memory>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>

namespace Mm
{

namespace Xml
{

namespace Base
{

namespace PugiXmlDetails
{

class AttributeWrapper
{
public:
  AttributeWrapper (pugi::xml_node p, pugi::xml_attribute a);

  pugi::xml_node parent;
  pugi::xml_attribute attr;
};

class AttributeTransform
{
public:
  AttributeTransform ();
  AttributeTransform (pugi::xml_node p);

  Attribute operator () (pugi::xml_attribute const& a) const;

private:
  pugi::xml_node parent;
};

using NodeTransform = Node (*) (pugi::xml_node const&);

using ChildIterator = boost::transform_iterator<NodeTransform, pugi::xml_node_iterator>;
using AttributeIterator = boost::transform_iterator<AttributeTransform, pugi::xml_attribute_iterator>;
using SiblingIterator = boost::transform_iterator<NodeTransform, pugi::xml_named_node_iterator>;

class WalkerWrapper final : public pugi::xml_tree_walker
{
public:
  WalkerWrapper (Walker& w);

private:
  virtual bool
  for_each (pugi::xml_node& node) override;

  virtual bool
  end (pugi::xml_node& node) override;

  bool
  run_postprocessing ();

  Walker& walker;
  std::stack<std::pair<Node, int>> nodes;
};

} // namespace PugiXmlDetails

class XmlImpl
{
public:
  using NodeImpl = pugi::xml_node;
  using AttributeImpl = PugiXmlDetails::AttributeWrapper;
  using DocumentImpl = std::unique_ptr<pugi::xml_document>;
  using WalkerImpl = PugiXmlDetails::WalkerWrapper;

  using ChildRange = boost::iterator_range<PugiXmlDetails::ChildIterator>;
  using AttributeRange = boost::iterator_range<PugiXmlDetails::AttributeIterator>;
  using SiblingRange = boost::iterator_range<PugiXmlDetails::SiblingIterator>;
};

namespace PugiXmlDetails
{

inline
AttributeWrapper::AttributeWrapper (pugi::xml_node p, pugi::xml_attribute a)
  : parent {std::move (p)},
    attr {std::move (a)}
{}

inline
AttributeTransform::AttributeTransform () = default;

inline
AttributeTransform::AttributeTransform (pugi::xml_node p)
  : parent {std::move (p)}
{}

inline Attribute
AttributeTransform::operator() (pugi::xml_attribute const& a) const
{
  return Attribute {AttributeWrapper {parent, a}};
}

inline
WalkerWrapper::WalkerWrapper (Walker& w)
  : walker {w},
    nodes {}
{}

inline bool
WalkerWrapper::for_each (pugi::xml_node& node)
{
  if (!run_postprocessing ())
  {
    return false;
  }

  Node n {node};
  auto d = depth ();
  nodes.push (std::make_pair (n, d));
  return walker.node (n, d);
}

inline bool
WalkerWrapper::end (pugi::xml_node&)
{
  return run_postprocessing ();
}

inline bool
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

} // namespace PugiXmlDetails

// node methods

template <>
inline std::string
Node::name () const
{
  return impl.name ();
}

template <>
inline std::experimental::optional<Node>
Node::parent () const
{
  auto p = impl.parent ();
  if (p)
  {
    return std::experimental::optional<Node> {std::experimental::in_place, std::move (p)};
  }

  return std::experimental::optional<Node> {};
}

template <>
inline std::experimental::optional<Node>
Node::child (std::string const& name) const
{
  auto c = impl.child (name.c_str ());
  if (c)
  {
    return std::experimental::optional<Node> {std::experimental::in_place, std::move (c)};
  }

  return std::experimental::optional<Node> {};
}

template <>
inline std::experimental::optional<Attribute>
Node::attribute (std::string const& name) const
{
  auto a = impl.attribute (name.c_str ());
  if (a)
  {
    XmlImpl::AttributeImpl attr_impl {this->impl, std::move (a)};
    return std::experimental::optional<Attribute> {std::experimental::in_place, std::move (attr_impl)};
  }

  return std::experimental::optional<Attribute> {};
}

template <>
inline std::string
Node::text () const
{
  return std::string {impl.text ().get ()};
}

template <>
inline bool
Node::is_tag () const
{
  return impl.type () == pugi::node_element;
}

template <>
inline XmlImpl::ChildRange
Node::children () const
{
  auto r = impl.children ();
  auto convert = [](pugi::xml_node const& node) { return Node {node}; };
  auto b = XmlImpl::ChildRange::iterator {r.begin (), convert};
  auto e = XmlImpl::ChildRange::iterator {r.end (), convert};
  return XmlImpl::ChildRange {b, e};
}


template <>
inline XmlImpl::AttributeRange
Node::attributes () const
{
  auto r = impl.attributes ();
  auto convert = PugiXmlDetails::AttributeTransform {impl};
  auto b = XmlImpl::AttributeRange::iterator {r.begin (), convert};
  auto e = XmlImpl::AttributeRange::iterator {r.end (), convert};
  return XmlImpl::AttributeRange {b, e};
}

template <>
inline XmlImpl::SiblingRange
Node::siblings (std::string const& name) const
{
  auto r = impl.parent ().children (name.c_str ());
  auto convert = [](pugi::xml_node const& node) { return Node {node}; };
  auto b = XmlImpl::SiblingRange::iterator {r.begin (), convert};
  auto e = XmlImpl::SiblingRange::iterator {r.end (), convert};
  return XmlImpl::SiblingRange {b, e};
}

template <>
inline Node
Node::add_child (std::string const& name)
{
  auto pnode = impl.append_child ();

  if (!pnode)
  {
    std::ostringstream oss;

    oss << "could not add a child named \"" << name << "\"";
    throw std::runtime_error {oss.str ()};
  }
  pnode.set_name (name.c_str ());
  return Node {pnode};
}

template <>
inline Attribute
Node::add_attribute (std::string const &name, std::string const& value)
{
  auto pattr = impl.append_attribute (name.c_str ());

  if (!pattr)
  {
    std::ostringstream oss;

    oss << "could not add an attribute named \"" << name << "\" with value \"" << value << "\"";
    throw std::runtime_error {oss.str ()};
  }
  pattr.set_value (value.c_str ());
  XmlImpl::AttributeImpl attr_impl {this->impl, std::move (pattr)};
  return Attribute {std::move (attr_impl)};
}

template <>
inline void
Node::set_text (std::string const& text)
{
  if (!impl.text ().set (text.c_str ()))
  {
    throw std::runtime_error {"could not set text"};
  }
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
Node::remove_text ()
{
  impl.remove_child (impl.text ().data ());
}

// attribute methods

template <>
inline std::string
Attribute::name () const
{
  return impl.attr.name ();
}

template <>
inline std::string
Attribute::value () const
{
  return impl.attr.value ();
}

template <>
inline Node
Attribute::parent () const
{
  return Node {impl.parent};
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
  doc.impl->traverse (impl);
}

// document methods

template <>
inline
Document::DocumentTmpl ()
  : impl {std::make_unique<pugi::xml_document> ()}
{}

template <>
inline
Document::DocumentTmpl (std::string const& filename)
  : Document {}
{
  auto result = impl->load_file (filename.c_str ());

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
  this->impl.swap (doc.impl);
  return *this;
}

template <>
inline Node
Document::as_node () const
{
  return Node {impl->root ()};
}

template <>
inline std::experimental::optional<Node>
Document::root_tag () const
{
  auto tag = impl->document_element ();
  if (tag)
  {
    return std::experimental::optional<Node> {std::experimental::in_place, std::move (tag)};
  }

  return std::experimental::optional<Node> {};
}

template <>
inline Node
Document::add_root (std::string const& name)
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
  return as_node ().add_child (name);
}

template <>
inline void
Document::save (std::ostream& os)
{
  impl->save (os, "  ");
}

} // namespace Base

} // namespace Xml

} // namespace Mm

#endif // MM_XML_BASE_XML_PUGI_HH
