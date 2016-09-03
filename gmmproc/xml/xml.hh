#ifndef GMMPROC_XML_XML_HH
#define GMMPROC_XML_XML_HH

#include "xmlfwd.hh"

#include <gmmproc/utils/exceptions.hh>

#include <iterator>
#include <type_traits>

// TODO: Add a debugging feature in bundle to track all document instances?
// TODO: libxml backend
// TODO: rapidxml backend
// TODO: more tests
// TODO: throw exceptions on errors
// TODO: split pugi impl to cc file?
// TODO: add a constexpr variable or something that tells which backend is used
// TODO: add context to the flying-by exceptions where applicable
// TODO: different directories for impls -> impls/<IMPL>/{prolog,epilog}.hh

// TODO: maybe add some convenience functions in a separate namespace (Xml::Extra)
// TODO: coverage
// TODO: move misc headers into some details subdirectory, keep {xml,xmlfwd,extra}.hh only
// TODO: rename the headers (get rid of the 2 in the name)
// TODO: fix the bogus "argh" exceptions
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

class XmlRuntimeError : public Utils::RuntimeError
{
public:
  using Utils::RuntimeError::RuntimeError;
};

class ParseError : public XmlRuntimeError
{
public:
  using XmlRuntimeError::XmlRuntimeError;
};

template <typename ImplP>
class BasicNodeTmpl
{
public:
  template <Utils::ViewType TypeV>
  static BasicNodeTmplView<ImplP, TypeV> create (typename ImplP::BasicNodeImpl i);

  DocumentTmplView<ImplP, Utils::ViewType::Mutable> document ();
  DocumentTmplView<ImplP, Utils::ViewType::Const> document () const;

  typename ImplP::template NodeRange<Utils::ViewType::Mutable> children ();
  typename ImplP::template NodeRange<Utils::ViewType::Const> children () const;

  void remove (NodeTmplView<ImplP, Utils::ViewType::Mutable> const& node);

  bool equal (BasicNodeTmpl const& rhs) const;

private:
  friend BasicNodeTmplView<ImplP, Utils::ViewType::Mutable>;
  friend BasicNodeTmplView<ImplP, Utils::ViewType::Const>;

  friend typename ImplP::template WalkerImpl<Utils::ViewType::Mutable>;
  friend typename ImplP::template WalkerImpl<Utils::ViewType::Const>;

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
  static AttributeTmplView<ImplP, TypeV> create (typename ImplP::AttributeImpl i);

  DocumentTmplView<ImplP, Utils::ViewType::Mutable> document ();
  DocumentTmplView<ImplP, Utils::ViewType::Const> document () const;

  Type::StringView name () const;
  void set_name (Type::StringView name);

  Type::StringView value () const;
  void set_value (Type::StringView value);

  NodeTmplView<ImplP, Utils::ViewType::Mutable> parent ();
  NodeTmplView<ImplP, Utils::ViewType::Const> parent () const;

  bool equal (AttributeTmpl const& other) const;

private:
  friend AttributeTmplView<ImplP, Utils::ViewType::Mutable>;
  friend AttributeTmplView<ImplP, Utils::ViewType::Const>;

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
  static TextTmplView<ImplP, TypeV> create (typename ImplP::TextImpl i);

  DocumentTmplView<ImplP, Utils::ViewType::Mutable> document ();
  DocumentTmplView<ImplP, Utils::ViewType::Const> document () const;

  Type::StringView text () const;
  void set_text (Type::StringView text);

  NodeTmplView<ImplP, Utils::ViewType::Mutable> parent ();
  NodeTmplView<ImplP, Utils::ViewType::Const> parent () const;

  TextType type () const;

  bool equal (TextTmpl const& other) const;

private:
  friend TextTmplView<ImplP, Utils::ViewType::Mutable>;
  friend TextTmplView<ImplP, Utils::ViewType::Const>;

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
  static NodeTmplView<ImplP, TypeV> create (typename ImplP::NodeImpl i);

  BasicNodeTmplView<ImplP, Utils::ViewType::Mutable> as_basic_node ();
  BasicNodeTmplView<ImplP, Utils::ViewType::Const> as_basic_node () const;

  DocumentTmplView<ImplP, Utils::ViewType::Mutable> document ();
  DocumentTmplView<ImplP, Utils::ViewType::Const> document () const;

  Type::StringView name () const;

  NodeOrDocTmpl<ImplP, Utils::ViewType::Mutable> parent ();
  NodeOrDocTmpl<ImplP, Utils::ViewType::Const> parent () const;
  BasicNodeTmplView<ImplP, Utils::ViewType::Mutable> basic_parent ();
  BasicNodeTmplView<ImplP, Utils::ViewType::Const> basic_parent () const;

  typename ImplP::template NodeRange<Utils::ViewType::Mutable> children ();
  typename ImplP::template NodeRange<Utils::ViewType::Const> children () const;

  typename ImplP::template AttributeRange<Utils::ViewType::Mutable> attributes ();
  typename ImplP::template AttributeRange<Utils::ViewType::Const> attributes () const;

  typename ImplP::template TextRange<Utils::ViewType::Mutable> texts ();
  typename ImplP::template TextRange<Utils::ViewType::Const> texts () const;

  typename ImplP::template NodeOrTextRange<Utils::ViewType::Mutable> all ();
  typename ImplP::template NodeOrTextRange<Utils::ViewType::Const> all () const;

  NodeTmplView<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::template NodeIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  NodeTmplView<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::template NodeIterator<Utils::ViewType::Const> pos, Type::StringView name);
  NodeTmplView<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::template TextIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  NodeTmplView<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::template TextIterator<Utils::ViewType::Const> pos, Type::StringView name);
  NodeTmplView<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::template NodeOrTextIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  NodeTmplView<ImplP, Utils::ViewType::Mutable> insert_child_at (typename ImplP::template NodeOrTextIterator<Utils::ViewType::Const> pos, Type::StringView name);

  AttributeTmplView<ImplP, Utils::ViewType::Mutable> insert_attribute_at (typename ImplP::template AttributeIterator<Utils::ViewType::Mutable> pos, Type::StringView name);
  AttributeTmplView<ImplP, Utils::ViewType::Mutable> insert_attribute_at (typename ImplP::template AttributeIterator<Utils::ViewType::Const> pos, Type::StringView name);

  TextTmplView<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::template NodeIterator<Utils::ViewType::Mutable> pos, TextType text_type = TextType::Parsed);
  TextTmplView<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::template NodeIterator<Utils::ViewType::Const> pos, TextType text_type = TextType::Parsed);
  TextTmplView<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::template TextIterator<Utils::ViewType::Mutable> pos, TextType text_type = TextType::Parsed);
  TextTmplView<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::template TextIterator<Utils::ViewType::Const> pos, TextType text_type = TextType::Parsed);
  TextTmplView<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::template NodeOrTextIterator<Utils::ViewType::Mutable> pos, TextType text_type = TextType::Parsed);
  TextTmplView<ImplP, Utils::ViewType::Mutable> insert_text_at (typename ImplP::template NodeOrTextIterator<Utils::ViewType::Const> pos, TextType text_type = TextType::Parsed);

  void remove (NodeTmplView<ImplP, Utils::ViewType::Mutable> const& node);
  void remove (AttributeTmplView<ImplP, Utils::ViewType::Mutable> const& attribute);
  void remove (TextTmplView<ImplP, Utils::ViewType::Mutable> const& text);

private:
  friend NodeTmplView<ImplP, Utils::ViewType::Mutable>;
  friend NodeTmplView<ImplP, Utils::ViewType::Const>;

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
  static DocumentTmplView<ImplP, TypeV> create (typename ImplP::DocumentImpl i);

  BasicNodeTmplView<ImplP, Utils::ViewType::Mutable> as_basic_node ();
  BasicNodeTmplView<ImplP, Utils::ViewType::Const> as_basic_node () const;

  Type::Optional<NodeTmplView<ImplP, Utils::ViewType::Mutable>> root_tag ();
  Type::Optional<NodeTmplView<ImplP, Utils::ViewType::Const>> root_tag () const;

  NodeTmplView<ImplP, Utils::ViewType::Mutable> add_root (Type::StringView name);

private:
  friend DocumentTmplView<ImplP, Utils::ViewType::Mutable>;
  friend DocumentTmplView<ImplP, Utils::ViewType::Const>;

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

  DocumentTmplView<ImplP, Utils::ViewType::Mutable> document ();
  DocumentTmplView<ImplP, Utils::ViewType::Const> document () const;

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
  virtual bool doc (DocumentTmplView<ImplP, TypeV>& doc, int depth) = 0;
  virtual bool node (NodeTmplView<ImplP, TypeV>& node, int depth) = 0;
  virtual bool text (TextTmplView<ImplP, TypeV>& text, int depth) = 0;
  virtual bool postprocess_node (NodeTmplView<ImplP, TypeV>& node, int depth) = 0;
  virtual bool postprocess_doc (DocumentTmplView<ImplP, TypeV>& doc, int depth) = 0;

  friend typename ImplP::template WalkerImpl<TypeV>;
};

} // namespace Xml

} // namespace Gmmproc

#define GMMPROC_XML_INCLUDING_IMPL

#if defined(GMMPROC_XML_USE_PUGI)

#include "impls/xml-pugi-epilog.hh"

#elif defined(GMMPROC_XML_USE_LIBXML)

#include "impls/xml-libxml-epilog.hh"

#else

#error "No xml implementation"

#endif

#undef GMMPROC_XML_INCLUDING_IMPL

#endif // GMMPROC_XML_XML_HH
