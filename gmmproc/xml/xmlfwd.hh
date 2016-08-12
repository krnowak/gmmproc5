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

#include <gmmproc/utils/wrapper.hh>

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
using Wrapper = Utils::Wrapper<TypeP>;

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
template <typename ImplP>
using NodeOrTextTmpl = Type::Variant<Type::Wrapper<NodeTmpl<ImplP>>, Type::Wrapper<TextTmpl<ImplP>>>;
template <typename ImplP>
using NodeOrTextConstTmpl = Type::Variant<Type::Wrapper<NodeTmpl<ImplP> const>, Type::Wrapper<TextTmpl<ImplP> const>>;
template <typename ImplP>
using NodeOrDocTmpl = Type::Variant<Type::Wrapper<NodeTmpl<ImplP>>, Type::Wrapper<DocumentTmpl<ImplP>>>;
template <typename ImplP>
using NodeOrDocConstTmpl = Type::Variant<Type::Wrapper<NodeTmpl<ImplP> const>, Type::Wrapper<DocumentTmpl<ImplP> const>>;

class XmlImpl;

using BasicNode = BasicNodeTmpl<XmlImpl>;
using Attribute = AttributeTmpl<XmlImpl>;
using Text = TextTmpl<XmlImpl>;
using Node = NodeTmpl<XmlImpl>;
using Document = DocumentTmpl<XmlImpl>;
using Bundle = BundleTmpl<XmlImpl>;
using NodeOrText = NodeOrTextTmpl<XmlImpl>;
using NodeOrTextConst = NodeOrTextConstTmpl<XmlImpl>;
using NodeOrDoc = NodeOrDocTmpl<XmlImpl>;
using NodeOrDocConst = NodeOrDocConstTmpl<XmlImpl>;

class ParseError;

enum class TextType
  {
    Parsed, // PCDATA
    Raw // CDATA
  };

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_XMLFWD_HH
