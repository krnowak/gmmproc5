#ifndef GMMPROC_XML_STRUCTURED_DETAIL_GROUP_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_GROUP_HH

#include <gmmproc/xml/structured/detail/storage.hh>
#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>
#include <gmmproc/xml/structured/getters.hh>

#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::Detail
{

template <typename GroupImplTypeP,
          typename AccessKeyP,
          typename PolicyP>
constexpr auto
wrap_in_forwarding_policy (GroupImplTypeP,
                           AccessKeyP,
                           PolicyP)
{
  return typename GroupImplTypeP::type::template ForwardingPolicy<AccessKeyP, PolicyP> {};
}

template <typename GroupImplTypeP,
          typename PartHanaTypeP>
constexpr auto
get_sub_access_info_from_part (GroupImplTypeP group_impl_type,
                               PartP part)
{
  auto access_info {API::Convenience::get_access_info (part)};
  auto sub_access_info_tuple {hana::fold_left
    (access_info.tuple,
     hana::make_tuple (),
     [group_impl_type](auto sub_access_info_tuple,
                       auto access_info_pair)
     {
       auto access_key {hana::first (access_info_pair)};
       auto getters_and_policies {hana::second (access_info_pair)};
       auto updated_getters_and_policies {hana::unpack
         (getters_and_policies,
          [group_impl_type, access_key](auto... getter_and_policy)
          {
            return hana::make_tuple
              (hana::make_pair
                (hana::first (getter_and_policy),
                 NoADL::wrap_in_forwarding_policy
                  (group_impl_type,
                   access_key,
                   hana::second (getter_and_policy)))...);
          })};

       return hana::append (sub_access_info_tuple,
                            hana::make_pair (access_key,
                                             updated_getters_and_policies));
     })};

  return NoADL::make_tuple_and_map (sub_access_info_tuple);
}

template <typename GroupP>
constexpr auto
get_sub_access_infos (GroupP group)
{
  auto group_impl_type {API::Convenience::get_type (group)};

  return hana::unpack
    (group.parts,
     [group_impl_type](auto... part_hana_type)
     {
       return hana::make_tuple
         (NoADL::get_sub_access_info_from_part (group_impl_type,
                                                hana::traits::declval (part_hana_type))...);
     });
}

} // Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_GROUP_HH
