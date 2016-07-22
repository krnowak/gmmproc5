#ifndef GMMPROC_XML_XML_HH
#define GMMPROC_XML_XML_HH

#include <experimental/optional>

#include <iosfwd>
#include <string>
#include <stdexcept>
#include <utility>

namespace Gmmproc
{

namespace Xml
{

template <typename Impl>
class NodeTmpl;
template <typename Impl>
class TextTmpl;
template <typename Impl>
class AttributeTmpl;
template <typename Impl>
class DocumentTmpl;
template <typename Impl>
class WalkerTmpl;

class XmlImpl;

using Node = NodeTmpl<XmlImpl>;
using Text = TextTmpl<XmlImpl>;
using Attribute = AttributeTmpl<XmlImpl>;
using Document = DocumentTmpl<XmlImpl>;
using Walker = WalkerTmpl<XmlImpl>;

class ParseError : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

enum class TextType
  {
    Plain, // PCDATA
    Special // CDATA
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

  typename Impl::ChildRange children () const;
  typename Impl::AttributeRange attributes () const;
  typename Impl::SiblingRange siblings (std::string const& name) const;
  typename Impl::TextRange texts () const;
  typename Impl::ChildTextRange all () const;

  NodeTmpl<Impl> add_child (std::string const& name);
  AttributeTmpl<Impl> add_attribute (std::string const& name,
                                     std::string const& value);
  TextTmpl<Impl> add_text (std::string const& text,
                           TextType text_type = TextType::Plain);

  void remove (NodeTmpl<Impl> const& child);
  void remove (AttributeTmpl<Impl> const &attribute);
  void remove (TextImpl<Impl> const& text);

private:
  typename Impl::NodeImpl impl;
};

template <typename Impl>
class TextTmpl
{
public:
  TextTmpl (typename Impl::TextImpl i)
    : impl {std::move (i)}
  {}

  std::string text () const;
  NodeTmpl<Impl> parent () const;
  TextType type () const;

private:
  friend class NodeTmpl<Impl>;
  typename Impl::TextImpl impl;
}

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
  virtual bool text (TextTmpl<Impl>& text, int depth) = 0;
  virtual bool postprocess_node (NodeTmpl<Impl>& node, int depth) = 0;

  friend typename Impl::WalkerImpl;
  typename Impl::WalkerImpl impl;
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

  std::experimental::optional<NodeTmpl<Impl>> root_tag () const;

  NodeTmpl<Impl> add_root (std::string const& name);
  void save (std::ostream& os);

private:
  friend class WalkerTmpl<Impl>;
  typename Impl::DocumentImpl impl;
};

} // namespace Xml

} // namespace Gmmproc

#define GMMPROC_XML_INCLUDING_IMPL

#if defined(GMMPROC_XML_USE_PUGI)

#include "xml-pugi.hh"

#elif defined(GMMPROC_XML_USE_LIBXML)

#include "xml-libxml.hh"

#else

#error "No xml implementation"

#endif

#undef GMMPROC_XML_INCLUDING_IMPL

#endif // GMMPROC_XML_XML_HH
