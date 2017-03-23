#ifndef GMMPROC_XML_STRUCTURED_API_HH
#define GMMPROC_XML_STRUCTURED_API_HH

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

template <typename ChildTagP,
          typename ChildP>
constexpr auto
get_type (ChildTagP tag,
          ChildP child)
{
  return NotOverridden {GetType {},
                        tag,
                        child};
}

class GetAccessInfo {};

template <typename ChildTagP,
          typename ChildP>
constexpr auto
get_access_info (ChildTagP tag,
                 ChildP child)
{
  return NotOverridden {GetAccessInfo {},
                        tag,
                        child};
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
          typename ContainerInfoP>
constexpr auto
get_getter_type (GetterTagP getter_tag,
                 ContainerInfoP container_info)
{
  return NotOverridden {GetGetterType {},
                        getter_tag,
                        container_info};
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
                   part);
}

template <typename PartP>
constexpr auto
get_access_info (PartP part)
{
  using API::get_access_info;

  return get_access_info (part.tag,
                          part);
}

} // namespace Convenience

} // namespace Gmmproc::Xml::Structured::API

#endif // GMMPROC_XML_STRUCTURED_API_HH
