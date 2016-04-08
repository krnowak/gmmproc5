#ifndef MM_GIR_XML_HH
#define MM_GIR_XML_HH

#include <mm/gir/names/names.hh>
#include <mm/xml/structured/node-base.hh>

#include <ext/kr/kr.hh>

namespace Mm
{

namespace Gir
{

// array-xor-type

using CInclude = Xml::Structured::NodeBase<Kr::TypeList<Xml::Structured::StringAttr<Names::Name>>, Kr::TypeList<>>;
using Doc = Xml::Structured::NodeBase<Kr::TypeList<Xml::Structured::StringAttr<Names::XmlSpace>>, Kr::TypeList<>, Xml::Structured::HasText>;

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_XML_HH
