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

template <typename NodeTagP>
constexpr auto
get_node_info (NodeTagP)
{
  // overloads/adl functions should return a hana::pair<name, type>
  // instead
  return Unregistered<NodeTagP>{};
}

} // namespace Gmmproc::Xml::Structured::Registry

#endif // GMMPROC_XML_STRUCTURED_REGISTRY_HH
