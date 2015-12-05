#ifndef MM_GIR_XML_HH
#define MM_GIR_XML_HH

#include <mm/gir/names/names.hh>
#include <mm/xml/node-base.hh>

#include <ext/kr/kr.hh>

namespace Mm
{

namespace Gir
{

// array-xor-type

using CInclude = Xml::NodeBase<Kr::TypeList<Xml::StringAttr<Names::Name>>, Kr::TypeList<>>;
using Doc = Xml::NodeBase<Kr::TypeList<Xml::StringAttr<Names::XmlSpace>>, Kr::TypeList<>, Xml::HasText>;

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_XML_HH
