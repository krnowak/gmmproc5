#ifndef GMMPROC_XML_STRUCTURED_EXTRA_DATA_PART_HH
#define GMMPROC_XML_STRUCTURED_EXTRA_DATA_PART_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>

#include <type_traits>

namespace Gmmproc::Xml::Structured
{

class ExtraDataTag
{};

template <typename AccessKeyP,
          typename ExtraDataImplP>
class ExtraData : public Part<ExtraDataTag>
{
public:
  using AccessKey = AccessKeyP;
  using ExtraDataImplType = boost::hana::type<ExtraDataImplP>;

  AccessKey access_key;
  ExtraDataImplType extra_data_impl_type;
};

template <typename ExtraDataTypeP>
constexpr auto
get_type (ExtraDataTag,
          ExtraDataTypeP extra_data_type)
{
  auto extra_data {boost::hana::traits::declval (extra_data_type)};

  return extra_data.extra_data_impl_type;
}

template <typename ExtraDataTypeP>
constexpr auto
get_access_info (ExtraDataTag,
                 ExtraDataTypeP extra_data_type)
{
  return Detail::get_simple_access_info (extra_data.access_key,
                                         SingleGetterTag {},
                                         PassThroughPolicy {});
}

constexpr auto
get_part_kind (ExtraDataTag)
{
  return PartKind::Extra;
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_EXTRA_DATA_PART_HH
