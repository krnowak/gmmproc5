#ifndef GMMPROC_XML_XMLFWD_HH
#define GMMPROC_XML_XMLFWD_HH

#define GMMPROC_XML_INCLUDING_IMPL

#if defined(GMMPROC_XML_USE_PUGI)

#include "xml-pugi2-prolog.hh"

#elif defined(GMMPROC_XML_USE_LIBXML)

#include "xml-libxml2-prolog.hh"

#else

#error "No xml implementation"

#endif

#undef GMMPROC_XML_INCLUDING_IMPL

#include <gmmproc/utils/view.hh>

#include <boost/variant.hpp>

#include <experimental/optional>
#include <experimental/string_view>

#include <functional>

namespace Gmmproc
{

namespace Xml
{

namespace Type
{

template <typename TypeP>
using Optional = std::experimental::optional<TypeP>;

using OptionalInPlace = std::experimental::in_place_t;

using StringView = std::experimental::string_view;

template <typename... TypesP>
using Variant = boost::variant<TypesP...>;

} // namespace Type

template <typename ImplP>
class BasicNodeTmpl;
template <typename ImplP>
class AttributeTmpl;
template <typename ImplP>
class TextTmpl;
template <typename ImplP>
class NodeTmpl;
template <typename ImplP>
class DocumentTmpl;
template <typename ImplP>
class BundleTmpl;

template <typename ImplP, Utils::ViewType TypeV>
using BasicNodeTmplView = Utils::ViewChoice<BasicNodeTmpl<ImplP>, TypeV>;
template <typename ImplP, Utils::ViewType TypeV>
using AttributeTmplView = Utils::ViewChoice<AttributeTmpl<ImplP>, TypeV>;
template <typename ImplP, Utils::ViewType TypeV>
using TextTmplView = Utils::ViewChoice<TextTmpl<ImplP>, TypeV>;
template <typename ImplP, Utils::ViewType TypeV>
using NodeTmplView = Utils::ViewChoice<NodeTmpl<ImplP>, TypeV>;
template <typename ImplP, Utils::ViewType TypeV>
using DocumentTmplView = Utils::ViewChoice<DocumentTmpl<ImplP>, TypeV>;

template <typename ImplP, Utils::ViewType TypeV>
using NodeOrTextTmpl = Type::Variant<NodeTmplView<ImplP, TypeV>, TextTmplView<ImplP, TypeV>>;
template <typename ImplP, Utils::ViewType TypeV>
using NodeOrDocTmpl = Type::Variant<NodeTmplView<ImplP, TypeV>, DocumentTmplView<ImplP, TypeV>>;

template <typename ImplP, Utils::ViewType TypeV>
class WalkerTmpl;

class XmlImpl;

using BasicNode = BasicNodeTmpl<XmlImpl>;
using Attribute = AttributeTmpl<XmlImpl>;
using Text = TextTmpl<XmlImpl>;
using Node = NodeTmpl<XmlImpl>;
using Document = DocumentTmpl<XmlImpl>;
using Bundle = BundleTmpl<XmlImpl>;

template <Utils::ViewType TypeV>
using BasicNodeViewChoice = BasicNodeTmplView<XmlImpl, TypeV>;
template <Utils::ViewType TypeV>
using AttributeViewChoice = AttributeTmplView<XmlImpl, TypeV>;
template <Utils::ViewType TypeV>
using TextViewChoice = TextTmplView<XmlImpl, TypeV>;
template <Utils::ViewType TypeV>
using NodeViewChoice = NodeTmplView<XmlImpl, TypeV>;
template <Utils::ViewType TypeV>
using DocumentViewChoice = DocumentTmplView<XmlImpl, TypeV>;
template <Utils::ViewType TypeV>
using NodeOrTextChoice = NodeOrTextTmpl<XmlImpl, TypeV>;
template <Utils::ViewType TypeV>
using NodeOrDocChoice = NodeOrDocTmpl<XmlImpl, TypeV>;

template <Utils::ViewType TypeV>
using WalkerChoice = WalkerTmpl<XmlImpl, TypeV>;

using BasicNodeView = BasicNodeViewChoice<Utils::ViewType::Mutable>;
using AttributeView = AttributeViewChoice<Utils::ViewType::Mutable>;
using TextView = TextViewChoice<Utils::ViewType::Mutable>;
using NodeView = NodeViewChoice<Utils::ViewType::Mutable>;
using DocumentView = DocumentViewChoice<Utils::ViewType::Mutable>;
using NodeOrDoc = NodeOrDocChoice<Utils::ViewType::Mutable>;
using NodeOrText = NodeOrTextChoice<Utils::ViewType::Mutable>;

using BasicNodeConstView = BasicNodeViewChoice<Utils::ViewType::Const>;
using AttributeConstView = AttributeViewChoice<Utils::ViewType::Const>;
using TextConstView = TextViewChoice<Utils::ViewType::Const>;
using NodeConstView = NodeViewChoice<Utils::ViewType::Const>;
using DocumentConstView = DocumentViewChoice<Utils::ViewType::Const>;
using NodeOrTextConst = NodeOrTextChoice<Utils::ViewType::Const>;
using NodeOrDocConst = NodeOrDocChoice<Utils::ViewType::Const>;

using Walker = WalkerChoice<Utils::ViewType::Mutable>;
using ConstWalker = WalkerChoice<Utils::ViewType::Const>;

class ParseError;

enum class TextType
  {
    Parsed, // PCDATA
    Raw // CDATA
  };

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_XMLFWD_HH
