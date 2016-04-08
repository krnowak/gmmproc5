#ifndef GMMPROC_GIR_XML_HH
#define GMMPROC_GIR_XML_HH

#include <gmmproc/gir/names/names.hh>
#include <gmmproc/xml/structured/node-base.hh>

#include <ext/kr/kr.hh>

namespace Gmmproc
{

namespace Gir
{

// array-xor-type

using CInclude = Xml::Structured::NodeBase<Kr::TypeList<Xml::Structured::StringAttr<Names::Name>>, Kr::TypeList<>>;
using Doc = Xml::Structured::NodeBase<Kr::TypeList<Xml::Structured::StringAttr<Names::XmlSpace>>, Kr::TypeList<>, Xml::Structured::HasText>;

} // namespace Gir

} // namespace Gmmproc

#endif // GMMPROC_GIR_XML_HH
