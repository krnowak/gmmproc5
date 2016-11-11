#ifndef GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH

#include <gmmproc/xml/structured/detail/mpl.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

class Flatten
{
public:
  template <typename SequenceP, typename OtherSequenceP>
  class Apply
  {
  public:
    using Type = Mpl::InsertRangeT<SequenceP, Mpl::EndT<SequenceP>, OtherSequenceP>;
  };
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_TYPE_HELPERS_HH
