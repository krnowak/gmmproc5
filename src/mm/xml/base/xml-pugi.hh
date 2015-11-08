#ifndef MM_XML_BASE_XML_PUGI_HH
#define MM_XML_BASE_XML_PUGI_HH

#ifndef MM_XML_BASE_INCLUDING_IMPL

#error This file should not be included directly

#endif

#include <sstream>

#include <pugixml.hpp>

#include <mm/utils/iterator-wrapper.hh>
#include <mm/utils/range.hh>

#include "xml.hh"

namespace Mm
{

namespace Xml
{

namespace Base
{

namespace PugiXmlDetails
{

} // namespace PugiXmlDetails

class XmlImpl
{
public:
  using NodeImpl = pugi::xml_node;
  class AttributeImpl
  {
  public:
    pugi::xml_node parent;
    pugi::xml_attribute attr;
  };
  using DocumentImpl = pugi::xml_document;

  using ChildRange = Utils::Range<Utils::IteratorWrapper<pugi::xml_node_iterator, Node>>;
  using SiblingsRange = Utils::Range<Utils::IteratorWrapper<pugi::xml_named_node_iterator, Node>>;
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
inline XmlImpl::ChildRange
Node::children() const
{
  auto r = impl.children ();
  return XmlImpl::ChildRange {r.begin (), r.end ()};
}

template <>
inline XmlImpl::SiblingsRange
Node::siblings(std::string const& name) const
{
  auto r = impl.parent ().children (name.c_str ());
  return XmlImpl::SiblingsRange {r.begin (), r.end ()};
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
