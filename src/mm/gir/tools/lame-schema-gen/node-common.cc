#include "node-common.hh"

#include <tuple>
#include <utility>

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

StrMap<std::size_t>
get_initial_occurences(Xml::Base::Node const& data_node)
{
  StrMap<std::size_t> occurences_per_child;

  for (auto const& child_node : data_node.children ())
  {
    ++occurences_per_child[child_node.name ()];
  }

  return occurences_per_child;
}

void
update_occurences (WithOccurences& wo,
                   std::size_t occurences)
{
  if (wo.min_occurences > occurences)
  {
    wo.min_occurences = occurences;
  }
  if (wo.max_occurences < occurences)
  {
    wo.max_occurences = occurences;
  }
}

void
update_attribute(Attribute const& attribute,
                 Counted const& child,
                 StrMap<StrSet>& unique)
{
  if ((attribute.count == child.count) && attribute.is_unique ())
  {
    unique.emplace (std::piecewise_construct,
                    std::forward_as_tuple (attribute.name),
                    std::forward_as_tuple ());
  }
}

void
get_attributes_uniqueness (Xml::Base::Node const& data_node,
                           UniqueAttributes& unique_attributes_per_child)
{
  for (auto const& child_node : data_node.children ())
  {
    auto const child_name = child_node.name ();
    auto& unique_attributes = unique_attributes_per_child[child_name];

    for (auto const& attribute : child_node.attributes ())
    {
      auto const name = attribute.name ();
      auto unique_iter = unique_attributes.find (name);

      if (unique_iter == unique_attributes.cend ())
      {
        continue;
      }

      auto& values_set = unique_iter->second;
      auto const value = attribute.value ();

      if (values_set.find (value) != values_set.cend ())
      {
        unique_attributes.erase (name);
        continue;
      }
      values_set.insert (value);
    }
  }
}

void
save_attribute (Attribute& attribute,
                StrMap<StrSet> const& unique_attributes)
{
  if (attribute.is_unique () &&
      (unique_attributes.find (attribute.name) == unique_attributes.cend ()))
  {
    attribute.mark_as_common ();
  }
}

} // namespace LameSchemaGenDetails

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
