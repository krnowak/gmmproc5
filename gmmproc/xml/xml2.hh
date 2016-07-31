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

template <typename ImplP>
class BasicNodeTmpl;
template <typename ImplP>
class NodeTmpl;
template <typename ImplP>
class TextTmpl;
template <typename ImplP>
class AttributeTmpl;
template <typename ImplP>
class DocumentTmpl;
template <typename ImplP>
class BundleTmpl;
template <typename ImplP>
class WalkerTmpl;
template <typename ImplP>
class ModWalkerTmpl;

class XmlImpl;

using BasicNode = BasicNodeTmpl<XmlImpl>;
using Node = NodeTmpl<XmlImpl>;
using Text = TextTmpl<XmlImpl>;
using Attribute = AttributeTmpl<XmlImpl>;
using Document = DocumentTmpl<XmlImpl>;
using Walker = WalkerTmpl<XmlImpl>;
using ModWalker = ModWalkerTmpl<XmlImpl>;
using Bundle = BundleTmpl<XmlImpl>;
using NodeOrText = Type::Variant<Type::Wrapper<Node>, Type::Wrapper<Text>>;
using NodeOrTextConst = Type::Variant<Type::Wrapper<Node const>, Type::Wrapper<Text const>>;
using NodeOrDoc = Type::Variant<Type::Wrapper<Node>, Type::Wrapper<Document>>;
using NodeOrDocConst = Type::Variant<Type::Wrapper<Node const>, Type::Wrapper<Document const>>;

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

template <typename ImplP>
class AttributeTmpl
{
public:
  AttributeTmpl (typename ImplP::AttributeImpl i)
    : impl {std::move (i)}
  {}

  ~AttributeTmpl ();

  Type::Wrapper<DocumentTmpl<ImplP>> document ();
  Type::Wrapper<DocumentTmpl<ImplP> const> document () const;

  Type::StringView name () const;
  void set_name (Type::StringView name);

  Type::StringView value () const;
  void set_value (Type::StringView value);

  Type::Wrapper<NodeTmpl<ImplP>> parent ();
  Type::Wrapper<NodeTmpl<ImplP> const> parent () const;

private:
  friend class Type::Wrapper<AttributeTmpl>;
  friend class Type::Wrapper<AttributeTmpl const>;

  AttributeTmpl (AttributeTmpl const& other);
  AttributeTmpl (AttributeTmpl&& other);

  AttributeTmpl& operator= (AttributeTmpl const& other);
  AttributeTmpl& operator= (AttributeTmpl&& other);

  typename ImplP::AttributeImpl impl;
};

template <typename ImplP>
class TextTmpl
{
public:
  TextTmpl (typename ImplP::TextImpl i)
    : impl {std::move (i)}
  {}

  ~TextTmpl ();

  Type::Wrapper<DocumentTmpl<ImplP>> document ();
  Type::Wrapper<DocumentTmpl<ImplP> const> document () const;

  Type::StringView text () const;
  void set_text (Type::StringView text);

  Type::Wrapper<NodeTmpl<ImplP>> parent ();
  Type::Wrapper<NodeTmpl<ImplP> const> parent () const;

  TextType type () const;

private:
  friend class Type::Wrapper<TextTmpl>;
  friend class Type::Wrapper<TextTmpl const>;

  TextTmpl (TextTmpl const& other);
  TextTmpl (TextTmpl&& other);

  TextTmpl& operator= (TextTmpl const& other);
  TextTmpl& operator= (TextTmpl&& other);

  typename ImplP::TextImpl impl;
};

template <typename ImplP>
class NodeTmpl
{
public:
  NodeTmpl (typename ImplP::NodeImpl i)
    impl {std::move (i)}
  {}

  ~NodeTmpl ();

  Type::Wrapper<BasicNodeTmpl<ImplP>> as_basic_node ();
  Type::Wrapper<BasicNodeTmpl<ImplP> const> as_basic_node () const;

  Type::Wrapper<DocumentTmpl<ImplP>> document ();
  Type::Wrapper<DocumentTmpl<ImplP> const> document () const;

  Type::StringView name () const;

  NodeOrDocTmpl<ImplP> parent ();
  NodeOrDocConstTmpl<ImplP> parent () const;
  Type::Wrapper<BasicNodeTmpl<ImplP>> basic_parent ();
  Type::Wrapper<BasicNodeTmpl<ImplP> const> basic_parent () const;

  Type::Opt<Type::Wrapper<NodeTmpl>> child (Type::StringView name);
  Type::Opt<Type::Wrapper<NodeTmpl const>> child (Type::StringView name) const;
  typename ImplP::NodeRange children ();
  typename ImplP::NodeConstRange children () const;
  typename ImplP::NodeRange children (Type::StringView name);
  typename ImplP::NodeConstRange children (Type::StringView name) const;

  Type::Opt<Type::Wrapper<AttributeTmpl<ImplP>>> attribute (StringView name);
  Type::Opt<Type::Wrapper<AttributeTmpl<ImplP> const>> attribute (StringView name) const;
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

  NodeTmpl (NodeTmpl const& other);
  NodeTmpl (NodeTmpl&& other);

  NodeTmpl& operator= (NodeTmpl const& other);
  NodeTmpl& operator= (NodeTmpl&& other);

  typename ImplP::NodeImpl impl;
};

template <typename ImplP>
class DocumentTmpl
{
public:
  DocumentTmpl (typename ImplP::DocumentImpl i)
    : impl {std::move (i)}
  {}

  ~DocumentTmpl();

  Type::Wrapper<BasicNode> as_basic_node ();
  Type::Wrapper<BasicNode const> as_basic_node () const;

  Type::Opt<Type::Wrapper<Node>> root_tag ();
  Type::Opt<Type::Wrapper<Node const>> root_tag () const;

  Type::Wrapper<Node> add_root (Type::StringView name);

private:
  friend class Type::Wrapper<DocumentTmpl>;
  friend class Type::Wrapper<DocumentTmpl const>;

  DocumentTmpl (DocumentTmpl const& other);
  DocumentTmpl (DocumentTmpl&& other);

  DocumentTmpl& operator= (DocumentTmpl const& other);
  DocumentTmpl& operator= (DocumentTmpl&& other);

  typename ImplP::DocumentImpl impl;
};

template <typename ImplP>
class BundleTmpl
{
public:
  BundleTmpl ();
  BundleTmpl (Type::StringView filename);

  ~BundleTmpl();

  BundleTmpl (BundleTmpl const& other) = delete;
  BundleTmpl (BundleTmpl&& other);

  BundleTmpl& operator= (BundleTmpl const& other) = delete;
  BundleTmpl& operator= (BundleTmpl&& other);

  void swap (BundleTmpl& other);

  Type::Wrapper<Document> document ();
  Type::Wrapper<Document const> document () const;

private:
  typename ImplP::BundleImpl impl;
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
