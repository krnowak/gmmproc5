#ifndef GMMPROC_XML_STRUCTURED_DETAIL_PART_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_PART_HH

#include <gmmproc/xml/structured/detail/utils.hh>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename GetterTagP,
          typename PolicyP>
constexpr auto
get_simple_getter_tags_and_policies (GetterTagP getter_tag,
                                     PolicyP policy)
{
  auto getter_tag_and_policy_pair {hana::make_pair (getter_tag,
                                                    policy)};

  return {NoADL::make_tuple_and_map (boost::hana::make_tuple (getter_tag_and_policy_pair))};
}

template <typename AccessKeyP,
          typename GetterTagsAndPoliciesP>
constexpr auto
get_simple_access_info (AccessKeyP access_key,
                        GetterTagsAndPoliciesP getter_tags_and_policies)
{
  auto access_info_pair {hana::make_pair (access_key,
                                          getter_tags_and_policies)};

  return NoADL::make_tuple_and_map (hana::make_tuple (access_info_pair));
}

template <typename AccessKeyP,
          typename GetterTagP,
          typename PolicyP>
constexpr auto
get_simple_access_info (AccessKeyP access_key,
                        GetterTagP getter_tag,
                        PolicyP policy)
{
  auto getter_tags_and_policies {get_simple_getter_tags_and_policies (getter_tag,
                                                                      policy)};

  return NoADL::get_simple_access_info (access_key,
                                        getter_tags_and_policies);
}

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_PART_HH
