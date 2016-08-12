#ifndef GMMPROC_XML_XML_HH
#define GMMPROC_XML_XML_HH

#define GMMPROC_XML_INCLUDING_IMPL

#if defined(GMMPROC_XML_USE_PUGI)

#include "xml-pugi2-prolog.hh"

#elif defined(GMMPROC_XML_USE_LIBXML)

#include "xml-libxml2-prolog.hh"

#else

#error "No xml implementation"

#endif

#undef GMMPROC_XML_INCLUDING_IMPL

#include "xmlfwd.hh"
#include "walkerfwd.hh"

#include <stdexcept>

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
  static Type::Wrapper<BasicNodeTmpl> create (typename ImplP::BasicNodeImpl i);
  static Type::Wrapper<BasicNodeTmpl const> create_const (typename ImplP::BasicNodeImpl i);

  Type::Wrapper<DocumentTmpl<ImplP>> document ();
  Type::Wrapper<DocumentTmpl<ImplP> const> document () const;

  Type::Optional<Type::Wrapper<NodeTmpl<ImplP>>> child (Type::StringView name);
  Type::Optional<Type::Wrapper<NodeTmpl<ImplP> const>> child (Type::StringView name) const;
  typename ImplP::NodeRange children ();
  typename ImplP::NodeConstRange children () const;
  typename ImplP::NodeRange children (Type::StringView name);
  typename ImplP::NodeConstRange children (Type::StringView name) const;

  void remove (Type::Wrapper<NodeTmpl<ImplP>> const& node);

  bool equal (BasicNodeTmpl const& rhs) const;

private:
  friend class Type::Wrapper<BasicNodeTmpl>;
  friend class Type::Wrapper<BasicNodeTmpl const>;

  BasicNodeTmpl (typename ImplP::BasicNodeImpl i);
  BasicNodeTmpl (BasicNodeTmpl const& other);
  BasicNodeTmpl (BasicNodeTmpl&& other) noexcept;

  ~BasicNodeTmpl () noexcept;

  BasicNodeTmpl& operator= (BasicNodeTmpl const& other);
  BasicNodeTmpl& operator= (BasicNodeTmpl&& other) noexcept;

  void swap (BasicNodeTmpl& other) noexcept;

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
  static Type::Wrapper<AttributeTmpl> create (typename ImplP::AttributeImpl i);
  static Type::Wrapper<AttributeTmpl const> create_const (typename ImplP::AttributeImpl i);

  Type::Wrapper<DocumentTmpl<ImplP>> document ();
  Type::Wrapper<DocumentTmpl<ImplP> const> document () const;

  Type::StringView name () const;
  void set_name (Type::StringView name);

  Type::StringView value () const;
  void set_value (Type::StringView value);

  Type::Wrapper<NodeTmpl<ImplP>> parent ();
  Type::Wrapper<NodeTmpl<ImplP> const> parent () const;

  bool equal (AttributeTmpl const& other) const;

private:
  friend class Type::Wrapper<AttributeTmpl>;
  friend class Type::Wrapper<AttributeTmpl const>;

  AttributeTmpl (typename ImplP::AttributeImpl i);
  AttributeTmpl (AttributeTmpl const& other);
  AttributeTmpl (AttributeTmpl&& other) noexcept;

  ~AttributeTmpl () noexcept;

  AttributeTmpl& operator= (AttributeTmpl const& other);
  AttributeTmpl& operator= (AttributeTmpl&& other) noexcept;

  void swap (AttributeTmpl& other) noexcept;

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
  static Type::Wrapper<TextTmpl> create (typename ImplP::TextImpl i);
  static Type::Wrapper<TextTmpl const> create_const (typename ImplP::TextImpl i);

  Type::Wrapper<DocumentTmpl<ImplP>> document ();
  Type::Wrapper<DocumentTmpl<ImplP> const> document () const;

  Type::StringView text () const;
  void set_text (Type::StringView text);

  Type::Wrapper<NodeTmpl<ImplP>> parent ();
  Type::Wrapper<NodeTmpl<ImplP> const> parent () const;

  TextType type () const;

  bool equal (TextTmpl const& other) const;

private:
  friend class Type::Wrapper<TextTmpl>;
  friend class Type::Wrapper<TextTmpl const>;

  TextTmpl (typename ImplP::TextImpl i);
  TextTmpl (TextTmpl const& other);
  TextTmpl (TextTmpl&& other) noexcept;

  ~TextTmpl () noexcept;

  TextTmpl& operator= (TextTmpl const& other);
  TextTmpl& operator= (TextTmpl&& other) noexcept;

  void swap (TextTmpl& other) noexcept;

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
  static Type::Wrapper<NodeTmpl> create (typename ImplP::NodeImpl i);
  static Type::Wrapper<NodeTmpl const> create_const (typename ImplP::NodeImpl i);

  Type::Wrapper<BasicNodeTmpl<ImplP>> as_basic_node ();
  Type::Wrapper<BasicNodeTmpl<ImplP> const> as_basic_node () const;

  Type::Wrapper<DocumentTmpl<ImplP>> document ();
  Type::Wrapper<DocumentTmpl<ImplP> const> document () const;

  Type::StringView name () const;

  NodeOrDocTmpl<ImplP> parent ();
  NodeOrDocConstTmpl<ImplP> parent () const;
  Type::Wrapper<BasicNodeTmpl<ImplP>> basic_parent ();
  Type::Wrapper<BasicNodeTmpl<ImplP> const> basic_parent () const;

  Type::Optional<Type::Wrapper<NodeTmpl>> child (Type::StringView name);
  Type::Optional<Type::Wrapper<NodeTmpl const>> child (Type::StringView name) const;
  typename ImplP::NodeRange children ();
  typename ImplP::NodeConstRange children () const;
  typename ImplP::NodeRange children (Type::StringView name);
  typename ImplP::NodeConstRange children (Type::StringView name) const;

  Type::Optional<Type::Wrapper<AttributeTmpl<ImplP>>> attribute (StringView name);
  Type::Optional<Type::Wrapper<AttributeTmpl<ImplP> const>> attribute (StringView name) const;
  typename ImplP::AttributeRange attributes ();
  typename ImplP::AttributeConstRange attributes () const;

  typename ImplP::TextRange texts ();
  typename ImplP::TextConstRange texts () const;

  typename ImplP::NodeOrTextRange all ();
  typename ImplP::NodeOrTextConstRange all () const;

  Type::Wrapper<NodeTmpl> add_child (Type::StringView name);
  Type::Wrapper<AttributeTmpl<ImplP>> add_attribute (Type::StringView name);
  Type::Wrapper<TextTmpl<ImplP>> add_text (TextType text_type = TextType::Parsed);

  void remove (Type::Wrapper<NodeTmpl> const& node);
  void remove (Type::Wrapper<AttributeTmpl<ImplP>> const& attribute);
  void remove (Type::Wrapper<TextTmpl<ImplP>> const& text);

private:
  friend class Type::Wrapper<NodeTmpl>;
  friend class Type::Wrapper<NodeTmpl const>;

  NodeTmpl (typename ImplP::NodeImpl i);
  NodeTmpl (NodeTmpl const& other);
  NodeTmpl (NodeTmpl&& other) noexcept;

  ~NodeTmpl () noexcept;

  NodeTmpl& operator= (NodeTmpl const& other);
  NodeTmpl& operator= (NodeTmpl&& other) noexcept;

  void swap (NodeTmpl& other) noexcept;

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
  return !(lhs == rhs)
}

template <typename ImplP>
class DocumentTmpl
{
public:
  static Type::Wrapper<DocumentTmpl> create (typename ImplP::DocumentImpl i);
  static Type::Wrapper<DocumentTmpl const> create_const (typename ImplP::DocumentImpl i);

  Type::Wrapper<BasicNode> as_basic_node ();
  Type::Wrapper<BasicNode const> as_basic_node () const;

  Type::Optional<Type::Wrapper<Node>> root_tag ();
  Type::Optional<Type::Wrapper<Node const>> root_tag () const;

  Type::Wrapper<Node> add_root (Type::StringView name);

private:
  friend class Type::Wrapper<DocumentTmpl>;
  friend class Type::Wrapper<DocumentTmpl const>;

  DocumentTmpl (typename ImplP::DocumentImpl i)
  DocumentTmpl (DocumentTmpl const& other);
  DocumentTmpl (DocumentTmpl&& other) noexcept;

  ~DocumentTmpl() noexcept;

  DocumentTmpl& operator= (DocumentTmpl const& other);
  DocumentTmpl& operator= (DocumentTmpl&& other) noexcept;

  void swap (DocumentTmpl& other) noexcept;

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

  BundleTmpl& operator= (BundleTmpl const& other) = delete;
  BundleTmpl& operator= (BundleTmpl&& other) noexcept;

  void swap (BundleTmpl& other) noexcept;

  Type::Wrapper<Document> document ();
  Type::Wrapper<Document const> document () const;

private:
  typename ImplP::BundleImpl impl;
};

template <typename ImplP, Helpers::WrapperType TypeV>
class WalkerTmpl
{
public:
  WalkerTmpl ();
  void walk (typename Helpers::CreatorTmpl<ImplP, TypeV>::NodeOrDocType node_or_doc) const;

private:
  virtual bool doc (typename Helpers::CreatorTmpl<ImplP, TypeV>::DocumentType& doc, int depth) const = 0;
  virtual bool node (typename Helpers::CreatorTmpl<ImplP, TypeV>::NodeType& node, int depth) const = 0;
  virtual bool text (typename Helpers::CreatorTmpl<ImplP, TypeV>::TextType& text, int depth) const = 0;
  virtual bool postprocess_node (typename Helpers::CreatorTmpl<ImplP, TypeV>::NodeType& node, int depth) const = 0;
  virtual bool postprocess_doc (typename Helpers::CreatorTmpl<ImplP, TypeV>::DocumentType& doc, int depth) const = 0;

  friend typename ImplP::WalkerImpl<TypeV>;
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
