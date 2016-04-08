#ifndef GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH

#include <gmmproc/xml/structured/detail/type-helpers.hh>

#include <boost/mpl/transform.hpp>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

class GetAttributeType
{
public:
  template <typename AttrP>
  class apply
  {
    using type = typename Attr::Foo;
  };
};

template <typename AttrListP>
using GetAttributeTypeListT = boost::mpl::transform<AttrListP, getattributetype, boost::mpl::back_inserter<boost::mpl::vector<>>,

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH
