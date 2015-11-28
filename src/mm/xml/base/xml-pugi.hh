#ifndef MM_XML_BASE_XML_PUGI_HH
#define MM_XML_BASE_XML_PUGI_HH

#ifndef MM_XML_BASE_INCLUDING_IMPL

#error This file should not be included directly

#endif

#include "xml.hh"

#include <pugixml.hpp>

#include <boost/range/iterator_range_core.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <functional>
#include <sstream>

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
  AttributeWrapper (pugi::xml_node p, pugi::xml_attribute a)
    : parent {std::move (p)},
      attr {std::move (a)}
  {}

  pugi::xml_node parent;
  pugi::xml_attribute attr;
};

using NodeTransform = Node(*)(pugi::xml_node const&);
// bloody lambda with a capture does not work with transform iterator
// from boost.
using AttributeTransform = std::function<Attribute(pugi::xml_attribute const&)>;

using ChildIterator = boost::transform_iterator<NodeTransform, pugi::xml_node_iterator>;
using AttributeIterator = boost::transform_iterator<AttributeTransform, pugi::xml_attribute_iterator>;
using SiblingIterator = boost::transform_iterator<NodeTransform, pugi::xml_named_node_iterator>;

} // namespace PugiXmlDetails

class XmlImpl
{
public:
  using NodeImpl = pugi::xml_node;
  using AttributeImpl = PugiXmlDetails::AttributeWrapper;
  using DocumentImpl = pugi::xml_document;

  using ChildRange = boost::iterator_range<PugiXmlDetails::ChildIterator>;
  using AttributeRange = boost::iterator_range<PugiXmlDetails::AttributeIterator>;
  using SiblingRange = boost::iterator_range<PugiXmlDetails::SiblingIterator>;
};

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
Node::attribute(std::string const& name) const
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
inline XmlImpl::ChildRange
Node::children() const
{
  auto r = impl.children ();
  auto convert = [](pugi::xml_node const& node) { return Node {node}; };
  auto b = XmlImpl::ChildRange::iterator {r.begin (), convert};
  auto e = XmlImpl::ChildRange::iterator {r.end (), convert};
  return XmlImpl::ChildRange {b, e};
}


template <>
inline XmlImpl::AttributeRange
Node::attributes() const
{
  throw 4;
  auto r = impl.attributes ();
  auto copy = impl;
  auto convert = [copy](pugi::xml_attribute const& attribute)
  {
    return Attribute {XmlImpl::AttributeImpl {pugi::xml_node{}, attribute}};
  };
  auto b = XmlImpl::AttributeRange::iterator {r.begin (), convert};
  auto e = XmlImpl::AttributeRange::iterator {r.end (), convert};
  return XmlImpl::AttributeRange {b, e};
}

template <>
inline XmlImpl::SiblingRange
Node::siblings(std::string const& name) const
{
  auto r = impl.parent ().children (name.c_str ());
  auto convert = [](pugi::xml_node const& node) { return Node {node}; };
  auto b = XmlImpl::SiblingRange::iterator {r.begin (), convert};
  auto e = XmlImpl::SiblingRange::iterator {r.end (), convert};
  return XmlImpl::SiblingRange {b, e};
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

// document methods

template <>
inline Document::DocumentTmpl (std::string const& filename)
  : impl {}
{
  auto result = impl.load_file (filename.c_str ());

  if (!result)
  {
    std::ostringstream oss;

    oss << "Failed to parse '" << filename << "': " << result.description ();
    throw ParseError(oss.str ());
  }
}

template <>
inline Node
Document::as_node () const
{
  return Node {impl.root ()};
}

} // namespace Base

} // namespace Xml

} // namespace Mm

#endif // MM_XML_BASE_XML_PUGI_HH
