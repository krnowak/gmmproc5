#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH

#include <gmmproc/xml/structured/detail/type-helpers.hh>
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
class Storage
{
private:
  using AllAttrTypes = GetAttributeTypeListT<AttrListP>;
  using AllChildContainerTypes = GetChildContainerTypeListT<ChildListP>;
  using AllTypes = GetAllTypesT<AllAttrTypes, AllChildContainerTypes, HasTextTypeP>;

public:
  using TupleType = GetStdTupleTypeT<AllTypes>;

protected:
  TupleType storage;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HH
