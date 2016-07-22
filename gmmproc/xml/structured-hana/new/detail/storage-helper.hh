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
  using PrivText = typename NodeSchemaP::Text;
  using PrivTextType = Mpl::ApplyT<typename PrivText::GetContainedType>;

  using PrivAttrList = typename NodeSchemaP::AttrList;
  using PrivChildList = typename NodeSchemaP::ChildList;
  using PrivTextList = std::conditional_t<std::is_same<void, PrivTextType>::value,
                                          Mpl::Vector<>,
                                          Mpl::Vector<PrivTextType>>;

  using PrivAttrTypes = GetPartTypeListT<PrivAttrList>;
  using PrivChildTypes = GetPartTypeListT<PrivChildList>;
  using PrivTextTypes = GetPartTypeListT<PrivTextList>;

  using PrivPartListTmp = Mpl::InsertRangeT<PrivAttrList,
                                            Mpl::EndT<PrivAttrList>,
                                            PrivChildList>;
  using PrivPartList = Mpl::InsertRangeT<PrivAttrListTmp,
                                         Mpl::EndT<PrivAttrListTmp>,
                                         PrivTextList>;
  using PrivPartTypes = GetPartTypeListT<PrivPartList>;

public:
  using AttrList = PrivAttrList;
  using ChildList = PrivChildList;
  using TextList = PrivTextList;
  using PartList = PrivPartList;

  using AttrTypes = PrivAttrTypes;
  using ChildTypes = PrivChildTypes;
  using TextTypes = PrivTextTypes;
  using PartTypes = PrivPartTypes;

  using TupleType = GetStdTupleTypeT<PartTypes>;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_STORAGE_HELPER_HH
