#ifndef GMMPROC_XML_STRUCTURED_API_CALL_HH
#define GMMPROC_XML_STRUCTURED_API_CALL_HH

#include <gmmproc/xml/structured/api.hh>

namespace Gmmproc::Xml::Structured::APICall
{

// part API

template <typename PartP>
constexpr auto
get_type (PartP part)
{
  using API::get_type;

  return get_type (part.tag, part);
}

template <typename PartP>
constexpr auto
get_access_info (PartP part)
{
  using API::get_access_info;

  return get_access_info (part.tag, part);
}

} // namespace Gmmproc::Xml::Structured::APICall

#endif // GMMPROC_XML_STRUCTURED_API_CALL_HH
