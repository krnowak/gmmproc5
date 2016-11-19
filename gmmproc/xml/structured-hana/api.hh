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

template <typename NameP, typename NodeHanaTypeP>
class Registered
{
public:
  using Name = NameP;
  using NodeHanaType = NodeHanaTypeP;

  constexpr
  Registered (Name n, NodeHanaType nht)
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

class GetContainedType {};

template <typename ChildTagP, typename ChildP>
constexpr auto
get_contained_type (ChildTagP tag, ChildP child)
{
  return NotOverridden {GetContainedType {}, tag, child};
}

class GetAccessInfo {};

template <typename ChildTagP, typename ChildP>
constexpr auto
get_access_info (ChildTagP tag, ChildP child)
{
  return NotOverridden {GetAccessInfo {}, tag, child};
}

/*

class GetGenerator {};

template <typename ChildP, typename StorageP>
constexpr auto
get_generator (ChildP child,
               boost::hana::basic_type<StorageP> storage)
{
  return NotOverridden {GetGenerator {}, child, storage};
}

*/

// storage API

class GetStorageType {};

template <typename StorageTagP, typename... TypeP>
constexpr auto
get_storage_type (StorageTagP storage_tag, TypeP... type)
{
  return NotOverridden {GetStorageType {}, storage_tag, type...};
}

} // namespace Gmmproc::Xml::Structured::API

#endif // GMMPROC_XML_STRUCTURED_API_HH
