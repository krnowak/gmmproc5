#ifndef GMMPROC_XML_XML_HH
#define GMMPROC_XML_XML_HH

#include "xmlfwd.hh"

#include <stdexcept>

// TODO: Add a debugging feature in bundle to track all document instances?
// TODO: maybe add some convenience functions in a separate namespace (XmlExtras)
// TODO: libxml backend
// TODO: more tests
// TODO: fix the bogus "argh" exceptions
// TODO: throw exceptions on errors

// TODO: split tests into multiple source files, not headers
// TODO: likely the virtual methods of the walker should not be const
// TODO: foorange -> foorange<utils::viewtype::mutable>
// TODO: fooconstrange -> foorange<utils::viewtype::const>
// TODO: rename create to create_mutable, add templated create (killed nontemplate methods)
// TODO: typedefs for wrappers
// TODO: move wrapper type to utils, have an template alias based on wrapper type enum
// TODO: rename Wrapper to View?
// TODO: replace add_foo(name_or_else) with lower-level add_foo_at (iterator, name_or_else)
// TODO: get rid of ::child accessor
// TODO: get rid of ::attribute accessor

namespace Gmmproc
{

namespace Xml
{

class ParseError : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};

template <typename ImplP>
class BasicNodeTmpl
{
public:
  template <Utils::ViewType TypeV>
  static BasicNodeTmplView<ImplP, TypeV> create (typename ImplP::BasicNodeImpl i);

  DocumentTmplView<ImplP, Utils::ViewType::Mutable> document ();
  DocumentTmplView<ImplP, Utils::ViewType::Const> document () const;

  typename ImplP::NodeRange<Utils::ViewType::Mutable> children ();
  typename ImplP::NodeRange<Utils::ViewType::Const> children () const;

  void remove (NodeViewTmpl<ImplP, Utils::ViewType::Mutable> const& node);

  bool equal (BasicNodeTmpl const& rhs) const;

private:
  template <Utils::ViewType TypeV>
  friend BasicNodeViewTmpl<ImplP, TypeV>;

  template <Utils::ViewType TypeV>
  friend typename ImplP::template WalkerImpl<TypeV>;

  BasicNodeTmpl (typename ImplP::BasicNodeImpl i);
  BasicNodeTmpl (BasicNodeTmpl const& other);
  BasicNodeTmpl (BasicNodeTmpl&& other) noexcept;

  ~BasicNodeTmpl () noexcept;

  void swap (BasicNodeTmpl& other) noexcept;

  BasicNodeTmpl& operator= (BasicNodeTmpl const& other);
  BasicNodeTmpl& operator= (BasicNodeTmpl&& other) noexcept;

  typename ImplP::BasicNodeImpl impl;
};

template <typename ImplP>
inline bool
operator== (BasicNodeTmpl<ImplP> const& lhs,
            BasicNodeTmpl<ImplP> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename ImplP>
inline bool
operator!= (BasicNodeTmpl<ImplP> const& lhs,
            BasicNodeTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
class AttributeTmpl
{
public:
  template <Utils::ViewType TypeV>
  static AttributeViewTmpl<ImplP, TypeV> create (typename ImplP::AttributeImpl i);

  DocumentViewTmpl<ImplP, Utils::ViewType::Mutable> document ();
  DocumentViewTmpl<ImplP, Utils::ViewType::Const> document () const;

  Type::StringView name () const;
  void set_name (Type::StringView name);

  Type::StringView value () const;
  void set_value (Type::StringView value);

  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> parent ();
  NodeViewTmpl<ImplP, Utils::ViewType::Const> parent () const;

  bool equal (AttributeTmpl const& other) const;

private:
  template <Utils::ViewType TypeV>
  friend AttributeViewTmpl<ImplP, TypeV>;

  friend NodeTmpl<ImplP>;

  AttributeTmpl (typename ImplP::AttributeImpl i);
  AttributeTmpl (AttributeTmpl const& other);
  AttributeTmpl (AttributeTmpl&& other) noexcept;

  ~AttributeTmpl () noexcept;

  void swap (AttributeTmpl& other) noexcept;

  AttributeTmpl& operator= (AttributeTmpl const& other);
  AttributeTmpl& operator= (AttributeTmpl&& other) noexcept;

  typename ImplP::AttributeImpl impl;
};

template <typename ImplP>
inline bool
operator== (AttributeTmpl<ImplP> const& lhs,
            AttributeTmpl<ImplP> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename ImplP>
inline bool
operator!= (AttributeTmpl<ImplP> const& lhs,
            AttributeTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
class TextTmpl
{
public:
  template <Utils::ViewType TypeV>
  static TextViewTmpl<ImplP, TypeV> create (typename ImplP::TextImpl i);

  DocumentViewTmpl<ImplP, Utils::ViewType::Mutable> document ();
  DocumentViewTmpl<ImplP, Utils::ViewType::Const> document () const;

  Type::StringView text () const;
  void set_text (Type::StringView text);

  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> parent ();
  NodeViewTmpl<ImplP, Utils::ViewType::Const> parent () const;

  TextType type () const;

  bool equal (TextTmpl const& other) const;

private:
  template <Utils::ViewType TypeV>
  friend TextViewTmpl<ImplP, TypeV>;

  friend NodeTmpl<ImplP>;

  TextTmpl (typename ImplP::TextImpl i);
  TextTmpl (TextTmpl const& other);
  TextTmpl (TextTmpl&& other) noexcept;

  ~TextTmpl () noexcept;

  void swap (TextTmpl& other) noexcept;

  TextTmpl& operator= (TextTmpl const& other);
  TextTmpl& operator= (TextTmpl&& other) noexcept;

  typename ImplP::TextImpl impl;
};

template <typename ImplP>
inline bool
operator== (TextTmpl<ImplP> const& lhs,
            TextTmpl<ImplP> const& rhs)
{
  return lhs.equal (rhs);
}

template <typename ImplP>
inline bool
operator!= (TextTmpl<ImplP> const& lhs,
            TextTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
class NodeTmpl
{
public:
  template <Utils::ViewType TypeV>
  static NodeViewTmpl<ImplP, TypeV> create (typename ImplP::NodeImpl i);

  BasicNodeViewTmpl<ImplP, Utils::ViewType::Mutable> as_basic_node ();
  BasicNodeViewTmpl<ImplP, Utils::ViewType::Const> as_basic_node () const;

  DocumentViewTmpl<ImplP, Utils::ViewType::Mutable> document ();
  DocumentViewTmpl<ImplP, Utils::ViewType::Const> document () const;

  Type::StringView name () const;

  NodeOrDocTmpl<ImplP, Utils::ViewType::Mutable> parent ();
  NodeOrDocTmpl<ImplP, Utils::ViewType::Const> parent () const;
  BasicNodeViewTmpl<ImplP, Utils::ViewType::Mutable> basic_parent ();
  BasicNodeViewTmpl<ImplP, Utils::ViewType::Const> basic_parent () const;

  typename ImplP::NodeRange<Utils::ViewType::Mutable> children ();
  typename ImplP::NodeRange<Utils::ViewType::Const> children () const;

  typename ImplP::AttributeRange<Utils::ViewType::Mutable> attributes ();
  typename ImplP::AttributeRange<Utils::ViewType::Const> attributes () const;

  typename ImplP::TextRange<Utils::ViewType::Mutable> texts ();
  typename ImplP::TextRange<Utils::ViewType::Const> texts () const;

  typename ImplP::NodeOrTextRange<Utils::ViewType::Mutable> all ();
  typename ImplP::NodeOrTextRange<Utils::ViewType::Const> all () const;

  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::NodeIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::NodeIterator<Utils::ViewType::Const> pos, Type::StringView name);
  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::TextIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::TextIterator<Utils::ViewType::Const> pos, Type::StringView name);
  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::NodeOrTextIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::NodeOrTextIterator<Utils::ViewType::Const> pos, Type::StringView name);

  AttributeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_attribute_at (typename ImplP::AttributeIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  AttributeViewTmpl<ImplP, Utils::ViewType::Mutable> insert_attribute_at (typename ImplP::AttributeIterator<Utils::ViewType::Const> pos, Type::StringView name);

  TextTmpl<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::NodeIterator<Utils::ViewType::Mutable> pos, TextType text_type = TextType::Parsed);
  TextTmpl<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::NodeIterator<Utils::ViewType::Const> pos, TextType text_type = TextType::Parsed);
  TextTmpl<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::TextIterator<Utils::ViewType::Mutable> pos, TextType text_type = TextType::Parsed);
  TextTmpl<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::TextIterator<Utils::ViewType::Const> pos, TextType text_type = TextType::Parsed);
  TextTmpl<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::NodeOrTextIterator<Utils::ViewType::Mutable> pos, TextType text_type = TextType::Parsed);
  TextTmpl<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::NodeOrTextIterator<Utils::ViewType::Const> pos, TextType text_type = TextType::Parsed);

  void remove (NodeViewTmpl<ImplP, Utils::ViewType::Mutable> const& node);
  void remove (AttributeViewTmpl<ImplP, Utils::ViewType::Mutable> const& attribute);
  void remove (TextViewTmpl<ImplP, Utils::ViewType::Mutable> const& text);

private:
  template <Utils::ViewType TypeV>
  friend NodeViewTmpl<ImplP, TypeV>;

  friend BasicNodeTmpl<ImplP>;

  NodeTmpl (typename ImplP::NodeImpl i);
  NodeTmpl (NodeTmpl const& other);
  NodeTmpl (NodeTmpl&& other) noexcept;

  ~NodeTmpl () noexcept;

  void swap (NodeTmpl& other) noexcept;

  NodeTmpl& operator= (NodeTmpl const& other);
  NodeTmpl& operator= (NodeTmpl&& other) noexcept;

  typename ImplP::NodeImpl impl;
};

template <typename ImplP>
inline bool
operator== (NodeTmpl<ImplP> const& lhs,
            NodeTmpl<ImplP> const& rhs)
{
  return lhs.as_basic_node () == rhs.as_basic_node ();
}

template <typename ImplP>
inline bool
operator!= (NodeTmpl<ImplP> const& lhs,
            NodeTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
inline bool
operator== (NodeTmpl<ImplP> const& lhs,
            BasicNodeTmpl<ImplP> const& rhs)
{
  return lhs.as_basic_node () == rhs;
}

template <typename ImplP>
inline bool
operator!= (NodeTmpl<ImplP> const& lhs,
            BasicNodeTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
inline bool
operator== (BasicNodeTmpl<ImplP> const& lhs,
            NodeTmpl<ImplP> const& rhs)
{
  return rhs == lhs;
}

template <typename ImplP>
inline bool
operator!= (BasicNodeTmpl<ImplP> const& lhs,
            NodeTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
class DocumentTmpl
{
public:
  template <Utils::ViewType TypeV>
  static DocumentViewTmpl<ImplP, TypeV> create (typename ImplP::DocumentImpl i);

  BasicNode<ImplP, Utils::ViewType::Mutable> as_basic_node ();
  BasicNode<ImplP, Utils::ViewType::Const> as_basic_node () const;

  Type::Optional<NodeViewTmpl<ImplP, Utils::ViewType::Mutable>> root_tag ();
  Type::Optional<NodeViewTmpl<ImplP, Utils::ViewType::Const>> root_tag () const;

  NodeViewTmpl<ImplP, Utils::ViewType::Mutable> add_root (Type::StringView name);

private:
  template <Utils::ViewType TypeV>
  friend DocumentViewTmpl<ImplP, TypeV>;

  DocumentTmpl (typename ImplP::DocumentImpl i);
  DocumentTmpl (DocumentTmpl const& other);
  DocumentTmpl (DocumentTmpl&& other) noexcept;

  ~DocumentTmpl() noexcept;

  void swap (DocumentTmpl& other) noexcept;

  DocumentTmpl& operator= (DocumentTmpl const& other);
  DocumentTmpl& operator= (DocumentTmpl&& other) noexcept;

  typename ImplP::DocumentImpl impl;
};

template <typename ImplP>
inline bool
operator== (DocumentTmpl<ImplP> const& lhs,
            DocumentTmpl<ImplP> const& rhs)
{
  return lhs.as_basic_node () == rhs.as_basic_node ();
}

template <typename ImplP>
inline bool
operator!= (DocumentTmpl<ImplP> const& lhs,
            DocumentTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
inline bool
operator== (DocumentTmpl<ImplP> const& lhs,
            BasicNodeTmpl<ImplP> const& rhs)
{
  return lhs.as_basic_node () == rhs;
}

template <typename ImplP>
inline bool
operator!= (DocumentTmpl<ImplP> const& lhs,
            BasicNodeTmpl<ImplP> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
inline bool
operator== (BasicNodeTmpl<ImplP> const& lhs,
            DocumentTmpl<ImplP> const& rhs)
{
  return rhs == lhs;
}

template <typename Impl>
inline bool
operator!= (BasicNodeTmpl<Impl> const& lhs,
            DocumentTmpl<Impl> const& rhs)
{
  return !(lhs == rhs);
}

template <typename ImplP>
class BundleTmpl
{
public:
  BundleTmpl ();
  BundleTmpl (Type::StringView filename);

  ~BundleTmpl() noexcept;

  BundleTmpl (BundleTmpl const& other) = delete;
  BundleTmpl (BundleTmpl&& other) noexcept;

  void swap (BundleTmpl& other) noexcept;

  BundleTmpl& operator= (BundleTmpl const& other) = delete;
  BundleTmpl& operator= (BundleTmpl&& other) noexcept;

  DocumentViewTmpl<ImplP, Utils::ViewType::Mutable> document ();
  DocumentViewTmpl<ImplP, Utils::ViewType::Const> document () const;

private:
  typename ImplP::BundleImpl impl;
};

template <typename ImplP, Utils::ViewType TypeV>
class WalkerTmpl
{
public:
  WalkerTmpl ();
  void walk (NodeOrDocTmpl<ImplP, TypeV> node_or_doc);

private:
  virtual bool doc (DocumentViewTmpl<ImplP, TypeV>& doc, int depth) = 0;
  virtual bool node (NodeViewTmpl<ImplP, TypeV>& node, int depth) = 0;
  virtual bool text (TextViewTmpl<ImplP, TypeV>& text, int depth) = 0;
  virtual bool postprocess_node (NodeViewTmpl<ImplP, TypeV>& node, int depth) = 0;
  virtual bool postprocess_doc (DocumentViewTmpl<ImplP, TypeV>& doc, int depth) = 0;

  friend typename ImplP::template WalkerImpl<TypeV>;
};

} // namespace Xml

} // namespace Gmmproc

#define GMMPROC_XML_INCLUDING_IMPL

#if defined(GMMPROC_XML_USE_PUGI)

#include "xml-pugi2-epilog.hh"

#elif defined(GMMPROC_XML_USE_LIBXML)

#include "xml-libxml2-epilog.hh"

#else

#error "No xml implementation"

#endif

#undef GMMPROC_XML_INCLUDING_IMPL

#endif // GMMPROC_XML_XML_HH
