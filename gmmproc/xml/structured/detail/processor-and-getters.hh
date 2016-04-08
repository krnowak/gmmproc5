#ifndef GMMPROC_XML_STRUCTURED_DETAIL_PROCESSOR_AND_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_PROCESSOR_AND_GETTERS_HH

#include <gmmproc/xml/structured/detail/storage.hh>
#include <gmmproc/xml/structured/child.hh>
#include <gmmproc/xml/structured/attribute.hh>

#include <boost/mpl/vector.hpp>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename AttrListP, typename ChildListP, typename HasTextTypeP>
class ProcessorAndGetters
{
  // TODO: Make sure that ChildListP has only unique child types.
private:
  using StorageType = Storage<AttrListP, ChildListP, HasTextTypeP>;

public:
  using Type = int;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_PROCESSOR_AND_GETTERS_HH
