#ifndef GMMPROC_XML_STRUCTURED_DETAIL_EXTRA_DATA_PART_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_EXTRA_DATA_PART_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>

#include <boost/hana/append.hpp>
#include <boost/hana/concat.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/fold_left.hpp>
#include <boost/hana/insert_range.hpp>
#include <boost/hana/keys.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/set.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/unpack.hpp>

#include <type_traits>

namespace Gmmproc::Xml::Structured::Detail
{

// TODO: move it outside the Detail namespace. add an ADL function
// like get_part_kind(tag), which returns an enum describing the kind
// of the part. it can be either a normal one or an extra one. normal
// ones can have custom access keys, can be a part of a group, gets an
// xml node or similar structured data on construction (or gets
// constructed with a default constructor if we do not pass any xml
// node or similar structured data). extra ones can not be a part of
// group, have hardcoded access key, which usually is even hidden from
// the developer, the hardcoded getter and gets constructed with a
// default constructor. check if we can somehow enforce the access key
// being hardcoded, make sure that the getter is actually hardcoded to
// some special single one (so we can even ignore the getter provided
// by some custom implementation). think if we want to be able to have
// more than one extra data member per node. if so, we can probably
// lift the hardcoded access key and a group ban limitations.

class ExtraDataTag
{};

class ExtraDataAccessKey
{};

template <typename ExtraDataImplTypeP>
class ExtraData : public Part<ExtraDataTag>
{
public:
  using ExtraDataImplType = ExtraDataImplTypeP;

  ExtraDataImplType extra_data_impl_type;
};

template <typename ChildTypeP>
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
                 ExtraDataTypeP)
{
  auto main_getter_tag_and_policy_pair = boost::hana::make_pair (Getters::ExtraDataGetterTag {},
                                                                 Getters::PassThroughPolicy {});
  auto main_getter_tags_and_policies {NoADL::make_tuple_and_map
    (boost::hana::make_tuple (main_getter_tag_and_policy_pair))};
  auto main_access_info_pair {boost::hana::make_pair (ExtraDataAccessKey {},
                                                      main_getter_tags_and_policies)};
  return NoADL::make_tuple_and_map
    (boost::hana::make_tuple (main_access_info_pair));
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_EXTRA_DATA_PART_HH
