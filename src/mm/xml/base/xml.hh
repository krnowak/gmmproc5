#ifndef MM_XML_BASE_XML_HH
#define MM_XML_BASE_XML_HH

#include <string>

#include <experimental/optional>

namespace Mm
{

namespace Xml
{

namespace Base
{

template <typename Impl>
class NodeTmpl;
template <typename Impl>
class AttributeTmpl;
template <typename Impl>
class DocumentTmpl;

class XmlImpl;

using Node = NodeTmpl<XmlImpl>;
using Attribute = AttributeTmpl<XmlImpl>;
using Document = DocumentTmpl<XmlImpl>;

class ParseError : std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

template <typename Impl>
class NodeTmpl
{
public:
  NodeTmpl (typename Impl::NodeImpl i)
    : impl (std::move (i))
  {}

  std::string name () const;
  std::experimental::optional<NodeTmpl<Impl>> parent () const;
  std::experimental::optional<NodeTmpl<Impl>> child (std::string const& name) const;
  std::experimental::optional<AttributeTmpl<Impl>> attribute(std::string const& name) const;
  std::string text () const;

  typename Impl::ChildRange children() const;
  typename Impl::AttributeRange attributes() const;
  typename Impl::SiblingRange siblings(std::string const& name) const;

  void remove (NodeTmpl<Impl> const& child);
  void remove (AttributeTmpl<Impl> const &attribute);
  void remove_text ();

private:
  typename Impl::NodeImpl impl;
};

template <typename Impl>
class AttributeTmpl
{
public:
  AttributeTmpl (typename Impl::AttributeImpl i)
    : impl (std::move (i))
  {}

  std::string name () const;
  std::string value () const;
  NodeTmpl<Impl> parent () const;

private:
  friend class NodeTmpl<Impl>;
  typename Impl::AttributeImpl impl;
};

template <typename Impl>
class DocumentTmpl
{
public:
  DocumentTmpl (std::string const& filename);
  NodeTmpl<Impl> as_node () const;

private:
  typename Impl::DocumentImpl impl;
};

} // namespace Base

} // namespace Xml

} // namespace Mm

#define MM_XML_BASE_USE_PUGI
#define MM_XML_BASE_INCLUDING_IMPL

#if defined(MM_XML_BASE_USE_PUGI)

#include "xml-pugi.hh"

/*

#elif defined(MM_XML_USE_LIBXML2)

#include "xml-libxml2.hh"

*/

#else

#error "No xml implementation"

#endif

#undef MM_XML_BASE_INCLUDING_IMPL

#endif // MM_XML_BASE_XML_HH
