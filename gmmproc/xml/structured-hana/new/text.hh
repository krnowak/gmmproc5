#ifndef GMMPROC_XML_STRUCTURED_TEXT_HH
#define GMMPROC_XML_STRUCTURED_TEXT_HH

#include <gmmproc/xml/structured/detail/text.hh>

#include <string>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Basic
{

using NoText = Detail::TextBase<void>;
using Text = Detail::TextBase<std::string>;

} // namespace Basic

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_TEXT_HH
