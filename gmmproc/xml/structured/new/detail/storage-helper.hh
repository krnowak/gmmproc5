#ifndef GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HELPER_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HELPER_HH

#include <gmmproc/xml/structured/detail/mpl.hh>
#include <gmmproc/xml/structured/detail/type-helpers.hh>

#include <string>
#include <type_traits>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename NodeSchemaP>
class StorageHelper
{
private:
  using PrivAttrList = typename NodeSchemaP::AttrList;
  using PrivChildList = typename NodeSchemaP::ChildList;
  using PrivPartList = Mpl::InsertRangeT<PrivAttrList,
                                         Mpl::EndT<PrivAttrList>,
                                         PrivChildList>;
  using PrivAttrTypes = GetPartTypeListT<PrivAttrList>;
  using PrivChildTypes = GetPartTypeListT<PrivChildList>;
  using PrivPartTypes = GetPartTypeListT<PrivPartList>;
  using PrivMaybeStringType = std::conditional_t<NodeSchemaP::HasText::value,
                                                 Mpl::Vector<std::string>,
                                                 Mpl::Vector<>>;
  using PrivAllTypes = Mpl::InsertRangeT<PrivPartTypes,
                                         Mpl::EndT<PrivPartTypes>,
                                         PrivMaybeStringType>;

public:
  using HasChild = typename NodeSchemaP::HasText;

  using AttrList = PrivAttrList;
  using ChildList = PrivChildList;
  using PartList = PrivPartList;

  using AttrTypes = PrivAttrTypes;
  using ChildTypes = PrivChildTypes;
  using PartTypes = PrivPartTypes;

  using AllTypes = PrivAllTypes;
  using TupleType = GetStdTupleTypeT<PrivAllTypes>;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HELPER_HH
