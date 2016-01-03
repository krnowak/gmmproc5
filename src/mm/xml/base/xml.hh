#ifndef MM_XML_BASE_XML_HH
#define MM_XML_BASE_XML_HH

#include <experimental/optional>

#include <iosfwd>
#include <string>

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
template <typename Impl>
class WalkerTmpl;

class XmlImpl;

using Node = NodeTmpl<XmlImpl>;
using Attribute = AttributeTmpl<XmlImpl>;
using Document = DocumentTmpl<XmlImpl>;
using Walker = WalkerTmpl<XmlImpl>;

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
    : impl {std::move (i)}
  {}

  std::string name () const;
  std::experimental::optional<NodeTmpl<Impl>> parent () const;
  std::experimental::optional<NodeTmpl<Impl>> child (std::string const& name) const;
  std::experimental::optional<AttributeTmpl<Impl>> attribute (std::string const& name) const;
  std::string text () const;
  bool is_tag () const;

  typename Impl::ChildRange children () const;
  typename Impl::AttributeRange attributes () const;
  typename Impl::SiblingRange siblings (std::string const& name) const;

  NodeTmpl<Impl> add_child (std::string const& name);
  AttributeTmpl<Impl> add_attribute (std::string const& name, std::string const& value);
  void set_text (std::string const& text);

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
    : impl {std::move (i)}
  {}

  std::string name () const;
  std::string value () const;
  NodeTmpl<Impl> parent () const;

private:
  friend class NodeTmpl<Impl>;
  typename Impl::AttributeImpl impl;
};

template <typename Impl>
class WalkerTmpl
{
public:
  WalkerTmpl ();
  void walk (DocumentTmpl<Impl>& doc);

private:
  virtual bool node (NodeTmpl<Impl>& node, int depth) = 0;
  virtual bool postprocess_node (NodeTmpl<Impl>& node, int depth) = 0;
  typename Impl::AttributeImpl impl;
};

template <typename Impl>
class DocumentTmpl
{
public:
  DocumentTmpl ();
  DocumentTmpl (std::string const& filename);
  DocumentTmpl (DocumentTmpl&& doc);
  DocumentTmpl (DocumentTmpl const& doc) = delete;

  DocumentTmpl& operator= (Document&& doc);
  DocumentTmpl& operator= (Document const& doc) = delete;

  NodeTmpl<Impl> as_node () const;
  std::experimental::optional<NodeTmpl<Impl>> root_tag () const;

  NodeTmpl<Impl> add_root (std::string const& name);
  void save (std::ostream& os);

private:
  friend class WalkerTmpl<Impl>;
  typename Impl::DocumentImpl impl;
};

} // namespace Base

} // namespace Xml

} // namespace Mm

#define MM_XML_BASE_INCLUDING_IMPL

#if defined(MM_XML_BASE_USE_PUGI)

#include "xml-pugi.hh"

#elif defined(MM_XML_BASE_USE_LIBXML)

#include "xml-libxml.hh"

#else

#error "No xml implementation"

#endif

#undef MM_XML_BASE_INCLUDING_IMPL

#endif // MM_XML_BASE_XML_HH
