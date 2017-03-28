#ifndef GMMPROC_XML_STRUCTURED_API_HH
#define GMMPROC_XML_STRUCTURED_API_HH

#include <boost/hana/type.hpp>

namespace Gmmproc::Xml::Structured::API
{

// common

template <typename... TypeP>
class NotOverridden
{
public:
  constexpr
  NotOverridden (TypeP...)
  {}
};

// registry API

template <typename NameP,
          typename NodeHanaTypeP>
class Registered
{
public:
  using Name = NameP;
  using NodeHanaType = NodeHanaTypeP;

  constexpr
  Registered (Name n,
              NodeHanaType nht)
  : name {std::move (n)},
    node_hana_type {std::move (nht)}
  {}

  Name name;
  NodeHanaTypeP node_hana_type;
};

class GetNodeInfo {};

template <typename NodeTagP>
constexpr auto
get_node_info (NodeTagP node_tag)
{
  // overloads/adl functions should return an instance of the Registered
  // template specialization instead:
  //
  // return Registered {some_name, some_node_hana_type};
  return NotOverridden {GetNodeInfo {}, node_tag};
}

// part API

class GetType {};

template <typename PartTagP,
          typename PartTypeP>
constexpr auto
get_type (PartTagP tag,
          PartTypeP part_type)
{
  return NotOverridden {GetType {},
                        tag,
                        part_type};
}

class GetAccessInfo {};

template <typename PartTagP,
          typename PartTypeP>
constexpr auto
get_access_info (PartTagP tag,
                 PartTypeP part_type)
{
  return NotOverridden {GetAccessInfo {},
                        tag,
                        part_type};
}

class GetPartKind {};

template <typename PartTagP>
constexpr auto
get_part_kind (PartTagP)
{
  return PartKind::Normal;
}

// storage API

class GetStorageType {};

template <typename StorageTagP,
          typename... TypeP>
constexpr auto
get_storage_type (StorageTagP storage_tag,
                  TypeP... type)
{
  return NotOverridden {GetStorageType {},
                        storage_tag,
                        type...};
}

// getter API

class GetGetterType {};

template <typename GetterTagP,
          typename ContainerInfoTypeP>
constexpr auto
get_getter_type (GetterTagP getter_tag,
                 ContainerInfoTypeP container_info_type)
{
  return NotOverridden {GetGetterType {},
                        getter_tag,
                        container_info_type};
}

// child API

class GetChildType {};

template <typename BasicP,
          typename NodeTagTypeP>
constexpr auto
get_child_type (BasicP basic,
                NodeTagTypeP node_tag_type)
{
  return NotOverridden {GetChildType {},
                        basic,
                        node_tag_type};
}

class GetChildGetterTagsAndPolicies {};

template <typename BasicP>
constexpr auto
get_child_getter_tags_and_policies (BasicP)
{
  return NotOverridden {GetChildGetterTagsAndPolicies {},
                        basic};
}

// value API
// TODO: finish it

template <typename WrapperP,
          typename PrimitiveP>
constexpr auto
get_wrapper_type (WrapperP wrapper,
                  PrimitiveP primitive)
{
  using API::get_wrapper_type;

  return get_wrapper_type (wrapper,
                           primitive);
}

template <typename WrapperP,
          typename PrimitiveP>
constexpr auto
get_wrapper_getter_tags_and_policies (WrapperP wrapper)
{
  using API::get_wrapper_getter_tags_and_policies;

  return get_wrapper_getter_tags_and_policies (wrapper);
}

/// A namespace for convenience functions.
///
/// They are never used as ADL functions.
namespace Convenience
{

// part API

template <typename PartP>
constexpr auto
get_type (PartP part)
{
  using API::get_type;

  return get_type (part.tag,
                   boost::hana::make_type (part));
}

template <typename PartP>
constexpr auto
get_access_info (PartP part)
{
  using API::get_access_info;

  return get_access_info (part.tag,
                          boost::hana::make_type (part));
}

template <typename PartTagP>
constexpr auto
get_part_kind (PartTagP tag)
{
  using API::get_part_kind;

  return get_part_kind (tag);
}

// storage API

template <typename StorageTagP,
          typename... TypeP>
constexpr auto
get_storage_type (StorageTagP storage_tag,
                  TypeP... type)
{
  using API::get_storage_type;

  return get_storage_type (storage_tag,
                           type...);
}

// getter API

template <typename GetterTagP,
          typename ContainerInfoP>
constexpr auto
get_getter_type (GetterTagP getter_tag,
                 ContainerInfoP container_info)
{
  using API::get_getter_type;

  return get_getter_type (getter_tag,
                          boost::hana::make_type (container_info));
}

// child API

template <typename BasicP,
          typename NodeTagTypeP>
constexpr auto
get_child_type (BasicP basic,
                NodeTagP node_tag)
{
  using API::get_child_type;

  return get_child_type (basic,
                         boost::hana::make_type (node_tag));
}

template <typename BasicP>
constexpr auto
get_child_getter_tags_and_policies (BasicP)
{
  using API::get_child_getter_tags_and_policies;

  return get_child_getter_tags_and_policies (basic);
}

// value API

template <typename WrapperP,
          typename PrimitiveP>
constexpr auto
get_wrapper_type (WrapperP wrapper,
                  PrimitiveP primitive)
{
  using API::get_wrapper_type;

  return get_wrapper_type (wrapper,
                           primitive);
}

template <typename WrapperP,
          typename PrimitiveP>
constexpr auto
get_wrapper_getter_tags_and_policies (WrapperP wrapper)
{
  using API::get_wrapper_getter_tags_and_policies;

  return get_wrapper_getter_tags_and_policies (wrapper);
}

} // namespace Convenience

} // namespace Gmmproc::Xml::Structured::API

#endif // GMMPROC_XML_STRUCTURED_API_HH
