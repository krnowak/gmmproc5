#ifndef GMMPROC_XML_XML_IMPL_HELPERS_HH
#define GMMPROC_XML_XML_IMPL_HELPERS_HH

#ifndef GMMPROC_XML_INCLUDING_IMPL

#error "This file should not be included directly"

#endif

#include "xmlfwd.hh"

namespace Gmmproc
{

namespace Xml
{

namespace Helpers
{

// wrapper type

enum class WrapperType
  {
    Mutable,
    Const
  };

template <typename ImplP, WrapperType TypeV>
class CreatorTmpl;

template <typename ImplP>
class CreatorTmpl<ImplP, WrapperType::Mutable>
{
public:
  using BasicNodeType = Type::Wrapper<BasicNodeTmpl<ImplP>>;
  using AttributeType = Type::Wrapper<AttributeTmpl<ImplP>>;
  using TextType = Type::Wrapper<TextTmpl<ImplP>>;
  using NodeType = Type::Wrapper<NodeTmpl<ImplP>>;
  using DocumentType = Type::Wrapper<DocumentTmpl<ImplP>>;
  using NodeOrTextType = Type::Variant<Type::Wrapper<NodeTmpl<ImplP>>, Type::Wrapper<TextTmpl<ImplP>>>;
  using NodeOrDocType = Type::Variant<Type::Wrapper<NodeTmpl<ImplP>>, Type::Wrapper<DocumentTmpl<ImplP>>>;

  static BasicNodeType
  create_basic_node (typename ImplP::BasicNodeImpl i)
  {
    return BasicNodeTmpl<ImplP>::create (std::move (i));
  }

  static AttributeType
  create_attribute (typename ImplP::AttributeImpl i)
  {
    return AttributeTmpl<ImplP>::create (std::move (i));
  }

  static TextType
  create_text (typename ImplP::TextImpl i)
  {
    return TextTmpl<ImplP>::create (std::move (i));
  }

  static NodeType
  create_node (typename ImplP::NodeImpl i)
  {
    return NodeTmpl<ImplP>::create (std::move (i));
  }

  static DocumentType
  create_document (typename ImplP::DocumentImpl i)
  {
    return DocumentTmpl<ImplP>::create (std::move (i));
  }
};

template <typename ImplP>
class CreatorTmpl<ImplP, WrapperType::Const>
{
public:
  using BasicNodeType = Type::Wrapper<BasicNodeTmpl<ImplP> const>;
  using AttributeType = Type::Wrapper<AttributeTmpl<ImplP> const>;
  using TextType = Type::Wrapper<TextTmpl<ImplP> const>;
  using NodeType = Type::Wrapper<NodeTmpl<ImplP> const>;
  using DocumentType = Type::Wrapper<DocumentTmpl<ImplP> const>;
  using NodeOrText = Type::Variant<Type::Wrapper<NodeTmpl<ImplP> const>, Type::Wrapper<TextTmpl<ImplP> const>>;
  using NodeOrDoc = Type::Variant<Type::Wrapper<NodeTmpl<ImplP> const>, Type::Wrapper<DocumentTmpl<ImplP> const>>;

  static BasicNodeType
  create_basic_node (typename ImplP::BasicNodeImpl i)
  {
    return BasicNodeTmpl<ImplP>::create_const (std::move (i));
  }

  static AttributeType
  create_attribute (typename ImplP::AttributeImpl i)
  {
    return AttributeTmpl<ImplP>::create_const (std::move (i));
  }

  static TextType
  create_text (typename ImplP::TextImpl i)
  {
    return TextTmpl<ImplP>::create_const (std::move (i));
  }

  static NodeType
  create_node (typename ImplP::NodeImpl i)
  {
    return NodeTmpl<ImplP>::create_const (std::move (i));
  }

  static DocumentType
  create_document (typename ImplP::DocumentImpl i)
  {
    return DocumentTmpl<ImplP>::create_const (std::move (i));
  }
};

template <WrapperType TypeV>
using Creator = CreatorTmpl<XmlImpl, TypeV>;

} // namespace Helpers

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_XML_IMPL_HELPERS_HH
