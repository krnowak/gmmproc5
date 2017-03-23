#ifndef GMMPROC_XML_STRUCTURED_REGISTRY_HH
#define GMMPROC_XML_STRUCTURED_REGISTRY_HH

namespace Gmmproc::Xml::Structured::Registry
{

/*
// exports Attribute, AttributeName
template <typename AttributeKeyP>
class AttrInfo;
*/

template <typename TypeP>
class Unregistered
{};

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

template <typename NodeTagP>
constexpr auto
get_node_info (NodeTagP)
{
  // overloads/adl functions should return an instance of the Registered
  // template specialization instead:
  //
  // return Registered {some_name, some_access_key};
  return Unregistered<NodeTagP>{};
}

} // namespace Gmmproc::Xml::Structured::Registry

#endif // GMMPROC_XML_STRUCTURED_REGISTRY_HH
