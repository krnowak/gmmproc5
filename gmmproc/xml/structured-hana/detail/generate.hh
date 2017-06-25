#ifndef GMMPROC_XML_STRUCTURED_DETAIL_GENERATE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_GENERATE_HH

#include <gmmproc/xml/structured/detail/utils.hh>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename StructuredTagP,
          typename PartP,
          typename... InputP>
decltype(auto)
generate_member (StructuredTagP structured_tag,
                 PartP part,
                 InputP&&... input)
{
  return API::Convenience::generate_part_member (part,
                                                 NoADL::wrap (structured_tag),
                                                 std::forward<Input> (input)...);
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_GENERATE_HH
