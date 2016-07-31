#ifndef GMMPROC_XML_XML_HH
#define GMMPROC_XML_XML_HH

#include <boost/variant.hpp>

#include <experimental/optional>
#include <experimental/string_view>

#include <functional>
#include <iosfwd>
#include <stdexcept>
#include <utility>

namespace Gmmproc
{

namespace Xml
{

namespace Type
{

template <typename TypeP>
class Wrapper
{
public:
  template <typename... TypesP>
  Wrapper (TypesP&& v...)
    : wrapped {v...}
  {}

  Wrapper (Wrapper<std::remove_const_t<TypeP>> const& other)
    : wrapped {other.wrapped}
  {}

  ~Wrapper() = default;

  Wrapper (Wrapper const& other) = default;
  Wrapper (Wrapper&& other) = default;

  Wrapper& operator= (Wrapper const& other) = default;
  Wrapper& operator= (Wrapper&& other) = default;

  void swap (Wrapper& other);

private:
  friend class Wrapper<const TypeP>;
  friend class Wrapper<std::remove_const_t<TypeP>>;

  TypeP wrapped;
};

template <typename TypeP>
using Optional = std::experimental::optional<TypeP>;

using OptionalInPlace = std::experimental::in_place_t;

using StringView = std::experimental::string_view;

template <typename... TypesP>
using Variant = boost::variant<TypesP...>;

template <typename TypeP>
using Ref = std::reference_wrapper<TypeP>;

} // namespace Type

template <typename Impl>
class BasicNodeTmpl;
template <typename Impl>
class NodeTmpl;
template <typename Impl>
class TextTmpl;
template <typename Impl>
class AttributeTmpl;
template <typename Impl>
class DocumentTmpl;
template <typename ImplP>
class BundleTmpl;
template <typename Impl>
class WalkerTmpl;

class XmlImpl;

using BasicNode = BasicNodeTmpl<XmlImpl>;
using Node = NodeTmpl<XmlImpl>;
using Text = TextTmpl<XmlImpl>;
using Attribute = AttributeTmpl<XmlImpl>;
using Document = DocumentTmpl<XmlImpl>;
using Walker = WalkerTmpl<XmlImpl>;
using NodeOrText = Type::Variant<Node, Text>;
using NodeOrDoc = Type::Variant<Node, Type::Ref<Document>>;

class ParseError : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

enum class TextType
  {
    Parsed, // PCDATA
    Raw // CDATA
  };

template <typename Impl>
class BasicNodeTmpl
{
public:
  BasicNodeTmpl (typename Impl::BasicNodeImpl i)
    : impl {std::move (i)}
  {}

  ~BasicNodeTmpl();

  BasicNodeTmpl (BasicNodeTmpl<Impl> const& basic_node) = delete;
  BasicNodeTmpl (BasicNodeTmpl<Impl>&& basic_node) = delete;

  BasicNodeTmpl<Impl>& operator= (BasicNodeTmpl const& basic_node) = delete;
  BasicNodeTmpl<Impl>& operator= (BasicNodeTmpl&& basic_node) = delete;

  DocumentTmpl<Impl> const& document () const;
  DocumentTmpl<Impl>& document ();

  Type::Optional<NodeTmpl<Impl>> child (Type::StringView name) const;
  typename Impl::NodeRange children () const;
  typename Impl::NodeRange children (Type::StringView name) const;

  void remove (NodeTmpl<Impl> const& child);

  bool equal (BasicNodeTmpl const& rhs) const;

private:
  friend class NodeTmpl<Impl>;
  friend class DocumentTmpl<Impl>;

  void swap (BasicNodeTmpl& base);

  typename Impl::BasicNodeImpl impl;
};

template <typename Impl>
inline bool
operator== (BasicNodeTmpl<Impl> const& lhs,
            BasicNodeTmpl<Impl> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename Impl>
inline bool
operator!= (BasicNodeTmpl<Impl> const& lhs,
            BasicNodeTmpl<Impl> const& rhs)
{

public:
  NodeTmpl (BasicNodeTmpl<Impl> b)
    : base {std::move (b)}
  {}

  ~NodeTmpl ();

  NodeTmpl (NodeTmpl const& node);
  NodeTmpl (NodeTmpl&& node);

  NodeTmpl& operator= (NodeTmpl const& node);
  NodeTmpl& operator= (NodeTmpl&& node);

  void swap (NodeTmpl& node);

  BasicNodeTmpl<Impl> const& as_basic_node () const;
  BasicNodeTmpl<Impl>& as_basic_node ();

  DocumentTmpl<Impl> const& document () const;
  DocumentTmpl<Impl>& document ();

  Type::StringView name () const;

  NodeOrDoc parent () const;
  BasicNodeTmpl<Impl> basic_parent() const;

  Type::Optional<NodeTmpl<Impl>> child (Type::StringView name) const;
  typename Impl::NodeRange children () const;
  typename Impl::NodeRange children (Type::StringView name) const;

  Type::Optional<AttributeTmpl<Impl>> attribute (Type::StringView name) const;
  typename Impl::AttributeRange attributes () const;

  typename Impl::TextRange texts () const;

  typename Impl::NodeOrTextRange all () const;

  NodeTmpl<Impl> add_child (Type::StringView name);
  AttributeTmpl<Impl> add_attribute (Type::StringView name,
                                     Type::StringView value);
  TextTmpl<Impl> add_text (Type::StringView text,
                           TextType text_type = TextType::Parsed);

  void remove (NodeTmpl<Impl> const& child);
  void remove (AttributeTmpl<Impl> const &attribute);
  void remove (TextTmpl<Impl> const& text);

private:
  BasicNodeTmpl<Impl> base;
};

template <typename Impl>
inline bool
operator== (NodeTmpl<Impl> const& lhs,
            NodeTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () == rhs.as_basic_node ();
}

template <typename Impl>
inline bool
operator!= (NodeTmpl<Impl> const& lhs,
            NodeTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () != rhs.as_basic_node ();
}

template <typename Impl>
inline bool
operator== (NodeTmpl<Impl> const& lhs,
            BasicNodeTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () == rhs;
}

template <typename Impl>
inline bool
operator!= (NodeTmpl<Impl> const& lhs,
            BasicNodeTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () != rhs;
}

template <typename Impl>
inline bool
operator== (BasicNodeTmpl<Impl> const& lhs,
            NodeTmpl<Impl> const& rhs)
{
  return rhs == lhs;
}

template <typename Impl>
inline bool
operator!= (BasicNodeTmpl<Impl> const& lhs,
            NodeTmpl<Impl> const& rhs)
{
  return rhs != lhs;
}

template <typename Impl>
class TextTmpl
{
public:
  TextTmpl (typename Impl::TextImpl i)
    : impl {std::move (i)}
  {}

  ~TextTmpl ();

  TextTmpl (TextTmpl const& text);
  TextTmpl (TextTmpl&& text);

  TextTmpl& operator= (TextTmpl const& text);
  TextTmpl& operator= (TextTmpl&& text);

  void swap (TextTmpl<Impl>& node);

  DocumentTmpl<Impl> const& document () const;
  DocumentTmpl<Impl>& document ();

  Type::StringView text () const;
  NodeTmpl<Impl> parent () const;
  TextType type () const;

  bool equal (TextTmpl const& rhs) const;

private:
  friend class NodeTmpl<Impl>;
  typename Impl::TextImpl impl;
};

template <typename Impl>
inline bool
operator== (TextTmpl<Impl> const& lhs,
            TextTmpl<Impl> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename Impl>
inline bool
operator!= (TextTmpl<Impl> const& lhs,
            TextTmpl<Impl> const& rhs)
{
  return !(lhs == rhs);
}

template <typename Impl>
class AttributeTmpl
{
public:
  AttributeTmpl (typename Impl::AttributeImpl i)
    : impl {std::move (i)}
  {}

  ~AttributeTmpl ();

  AttributeTmpl (AttributeTmpl const& attribute);
  AttributeTmpl (AttributeTmpl&& attribute);

  AttributeTmpl& operator= (AttributeTmpl const& attribute);
  AttributeTmpl& operator= (AttributeTmpl&& attribute);

  void swap (AttributeTmpl<Impl>& node);

  DocumentTmpl<Impl> const& document () const;
  DocumentTmpl<Impl>& document ();

  Type::StringView name () const;
  Type::StringView value () const;
  NodeTmpl<Impl> parent () const;

  bool equal (AttributeTmpl const& rhs) const;

private:
  friend class NodeTmpl<Impl>;
  typename Impl::AttributeImpl impl;
};

template <typename Impl>
inline bool
operator== (AttributeTmpl<Impl> const& lhs,
            AttributeTmpl<Impl> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename Impl>
inline bool
operator!= (AttributeTmpl<Impl> const& lhs,
            AttributeTmpl<Impl> const& rhs)
{
  return !(lhs == rhs);
}

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
class DocumentViewTmpl
{
public:
  DocumentViewTmpl (DocumentViewTmpl<Impl>&& doc) = default;
  DocumentViewTmpl (DocumentViewTmpl<Impl> const& doc) = default;

  DocumentViewTmpl& operator= (DocumentViewTmpl<Impl>&& doc) = default;
  DocumentViewTmpl& operator= (DocumentViewTmpl<Impl> const& doc) = default;

  BasicNodeTmpl<Impl> const& as_basic_node () const;
  BasicNodeTmpl<Impl>& as_basic_node ();

  Type::Optional<NodeTmpl<Impl>> root_tag () const;

  NodeTmpl<Impl> add_root (Type::StringView name);
  void save (std::ostream& os);
};

template <typename Impl>
class DocumentTmpl
{
public:
  DocumentTmpl ();
  DocumentTmpl (Type::StringView filename);
  DocumentTmpl (DocumentTmpl&& doc);
  DocumentTmpl (DocumentTmpl const& doc) = delete;

  DocumentTmpl& operator= (Document&& doc);
  DocumentTmpl& operator= (Document const& doc) = delete;

  BasicNodeTmpl<Impl>
  as_basic_node () const;

  Type::Optional<NodeTmpl<Impl>> root_tag () const;

  NodeTmpl<Impl> add_root (Type::StringView name);
  void save (std::ostream& os);

private:
  friend class NodeTmpl<Impl>;
  friend class WalkerTmpl<Impl>;

  typename Impl::DocumentImpl impl;
};

template <typename Impl>
inline bool
operator== (DocumentTmpl<Impl> const& lhs,
            DocumentTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () == rhs.as_basic_node ();
}

template <typename Impl>
inline bool
operator!= (DocumentTmpl<Impl> const& lhs,
            DocumentTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () != rhs.as_basic_node ();
}

template <typename Impl>
inline bool
operator== (DocumentTmpl<Impl> const& lhs,
            BasicNodeTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () == rhs;
}

template <typename Impl>
inline bool
operator!= (DocumentTmpl<Impl> const& lhs,
            BasicNodeTmpl<Impl> const& rhs)
{
  return lhs.as_basic_node () != rhs;
}

template <typename Impl>
inline bool
operator== (BasicNodeTmpl<Impl> const& lhs,
            DocumentTmpl<Impl> const& rhs)
{
  return rhs == lhs;
}

template <typename Impl>
inline bool
operator!= (BasicNodeTmpl<Impl> const& lhs,
            DocumentTmpl<Impl> const& rhs)
{
  return rhs != lhs;
}

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
