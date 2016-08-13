#ifndef GMMPROC_XML_WALKERFWD_HH
#define GMMPROC_XML_WALKERFWD_HH

#include "xmlfwd.hh"

#define GMMPROC_XML_INCLUDING_IMPL

#include "xml-impl-helpers.hh"

#undef GMMPROC_XML_INCLUDING_IMPL

namespace Gmmproc
{

namespace Xml
{

template <typename ImplP, Helpers::WrapperType TypeV>
class WalkerTmpl;
template <Helpers::WrapperType TypeV>
using Walker = WalkerTmpl<XmlImpl, TypeV>;
using ConstWalker = Walker<Helpers::WrapperType::Const>;
using MutableWalker = Walker<Helpers::WrapperType::Mutable>;

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_WALKERFWD_HH
