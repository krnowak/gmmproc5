#ifndef MM_GIR_TOOLS_LAME_SCHEMA_GEN_NODE_COMMON_HH
#define MM_GIR_TOOLS_LAME_SCHEMA_GEN_NODE_COMMON_HH

#include "types.hh"
#include "utils.hh"

#include <mm/xml/base/xml.hh>

#include <cstddef>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

namespace LameSchemaGenDetails
{

template <typename Node>
class NodeWrapperModel
{
public:
  template <typename Element> class RefRangeType;

  RefRangeType<Attribute>
  attributes ();
  RefRangeType<const Attribute>
  attributes () const;

  RefRangeType<NodeWrapperModel<Node>>
  children ();
  RefRangeType<NodeWrapperModel<const Node>>
  children () const;

  Named&
  as_named();
  Named const&
  as_named() const;

  WithOccurences&
  as_with_occurences ();
  WithOccurences const&
  as_with_occurences () const;

  Counted&
  as_counted ();
  Counted const&
  as_counted () const;
};

StrMap<std::size_t>
get_initial_occurences(Xml::Base::Node const& data_node);

void
update_occurences (WithOccurences& wo,
                   std::size_t occurences);

using UniqueAttributes = StrMap<StrMap<StrSet>>;

void
update_attribute(Attribute const& attribute,
                 Counted const& child,
                 StrMap<StrSet>& unique);

template <typename NodeWrapper>
UniqueAttributes
get_initial_attribute_maps (NodeWrapper const& data_wrapper)
{
  UniqueAttributes unique_attributes_per_child;

  for (auto child_wrapper : data_wrapper.children ())
  {
    auto const& named_child = child_wrapper.as_named ();
    auto& unique_attributes = unique_attributes_per_child[named_child.name];
    auto const& counted_child = child_wrapper.as_counted ();

    for (auto const& attribute : child_wrapper.attributes ())
    {
      update_attribute (attribute, counted_child, unique_attributes);
    }
  }

  return unique_attributes_per_child;
}

void
get_attributes_uniqueness (Xml::Base::Node const& data_node,
                           UniqueAttributes& unique_attributes_per_child);

void
save_attribute (Attribute& attribute,
                StrMap<StrSet> const& unique_attributes);

template <typename NodeWrapper>
void
save_attribute_uniqueness (NodeWrapper& data_wrapper,
                           UniqueAttributes const& unique_attributes_per_child)
{
  for (auto child_wrapper : data_wrapper.children ())
  {
    auto const& named_child = child_wrapper.as_named ();
    auto const& unique_attributes = must_get (unique_attributes_per_child, named_child.name);

    for (auto& attribute : child_wrapper.attributes ())
    {
      save_attribute (attribute, unique_attributes);
    }
  }
}

} // namespace LameSchemaGenDetails

template <typename NodeWrapper>
void
get_occurences (NodeWrapper& data_wrapper,
                Xml::Base::Node const& data_node)
{
  auto occurences_per_child = LameSchemaGenDetails::get_initial_occurences (data_node);

  for (auto child_wrapper : data_wrapper.children ())
  {
    auto const& named_child = child_wrapper.as_named ();
    auto const occurences = occurences_per_child[named_child.name];
    auto& child_with_occurences = child_wrapper.as_with_occurences ();

    LameSchemaGenDetails::update_occurences (child_with_occurences, occurences);
  }
}

template <typename NodeWrapper>
void
get_unique_attributes (NodeWrapper& data_wrapper,
                       Xml::Base::Node const& data_node)
{
  auto unique_attributes_per_child = LameSchemaGenDetails::get_initial_attribute_maps (data_wrapper);

  LameSchemaGenDetails::get_attributes_uniqueness (data_node, unique_attributes_per_child);
  LameSchemaGenDetails::save_attribute_uniqueness (data_wrapper, unique_attributes_per_child);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm

#endif // MM_GIR_TOOLS_LAME_SCHEMA_GEN_NODE_COMMON_HH
