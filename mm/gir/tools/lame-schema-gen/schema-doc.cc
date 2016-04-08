#include "schema-doc.hh"
#include "schema-data.hh"
#include "types.hh"
#include "utils.hh"

#include <mm/xml/xml.hh>

#include <boost/algorithm/string/join.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <queue>
#include <utility>

namespace Mm
{

namespace Gir
{

namespace Tools
{

namespace LameSchemaGen
{

namespace
{

template <typename Iterator>
StrVector
get_sorted_strings (Iterator first,
                    Iterator const& last)
{
  StrVector sorted;

  sorted.reserve (std::distance (first, last));
  std::copy (first, last, std::back_inserter (sorted));
  std::sort (sorted.begin (), sorted.end ());
  return sorted;
}

template <typename Cont>
StrVector
get_sorted_strings_from_cont (Cont const& c)
{
  return get_sorted_strings (std::begin (c), std::end (c));

}

void
build_names (Xml::Node& root,
             StrSet const& names_set)
{
  auto names_node = root.add_child ("names");
  auto const sorted = get_sorted_strings_from_cont (names_set);

  for (auto const& name : sorted)
  {
    auto name_node = names_node.add_child ("name");
    name_node.add_attribute ("name", name);
  }
}

template <typename ValueType>
StrVector
get_sorted_strings_from_strmap (StrMap<ValueType> const& m)
{
  return get_sorted_strings_from_cont (get_map_range<0> (m));
}

enum class ChildType
{
  SINGLE,
  OPTIONAL,
  MULTI
};

ChildType
get_child_type (WithOccurences const& child)
{
  if (child.min_occurences == 1 && child.max_occurences == 1)
  {
    return ChildType::SINGLE;
  }
  if (child.min_occurences == 0 && child.max_occurences == 1)
  {
    return ChildType::OPTIONAL;
  }
  return ChildType::MULTI;
}

class UniqueAttributePredicate
{
public:
  bool
  operator() (StrMap<Attribute>::value_type const& p) const
  {
    return p.second.is_unique ();
  }
};

StrVector
get_unique_attribute_names (StrMap<Attribute> const& attributes)
{
  UniqueAttributePredicate pred;
  auto fb = boost::make_filter_iterator (pred, attributes.begin (), attributes.end ());
  auto fe = boost::make_filter_iterator (pred, attributes.end (), attributes.end ());
  return get_sorted_strings (get_map_iterator<0> (std::move (fb)),
                             get_map_iterator<0> (std::move (fe)));
}

void
set_short_child_type_attributes (ShortNode const& parent,
                                 ShortNode::Child const& child,
                                 ShortNode const& full_child,
                                 Xml::Node& child_node)
{
  switch (get_child_type (child))
  {
  case ChildType::SINGLE:
    child_node.add_attribute ("type", "single");
    break;

  case ChildType::OPTIONAL:
    {
      bool const recursion_point = must_get (full_child.parents, parent.name);

      child_node.add_attribute ("type", "optional");
      child_node.add_attribute ("extra-type-info", recursion_point ? "ptr" : "opt");
    }
    break;

  case ChildType::MULTI:
    {
      auto unique_attrs = get_unique_attribute_names (full_child.attributes);
      if (unique_attrs.empty ())
      {
        child_node.add_attribute ("type", "vector");
      }
      else
      {
        child_node.add_attribute ("type", "map-vector");
        child_node.add_attribute ("extra-type-info", boost::algorithm::join (unique_attrs, "|"));
      }
    }
    break;
  }
}

void
build_short_attributes (ShortNode const& data,
                        Xml::Node& element_node)
{
  auto attributes_node = element_node.add_child ("attributes");
  auto attributes_listing_node = attributes_node.add_child ("listing");
  auto const sorted_attrs = get_sorted_strings_from_strmap (data.attributes);

  for (auto const& attr_name : sorted_attrs)
  {
    auto const& attr = must_get (data.attributes, attr_name);
    auto attribute_node = attributes_listing_node.add_child ("attribute");

    attribute_node.add_attribute ("name", attr_name);
    attribute_node.add_attribute ("type", attr.to_string ());
    attribute_node.add_attribute ("unique", attr.is_unique() ? "1" : "0");
  }
}

void
build_short_children (StrMap<ShortNode> const& short_data,
                      ShortNode const& data,
                      Xml::Node& element_node)
{
  auto children_node = element_node.add_child ("children");
  auto children_listing_node = children_node.add_child ("listing");
  auto children_common_node = children_node.add_child ("common");
  auto children_exclusives_node = children_node.add_child ("exclusives");
  auto const sorted_children = get_sorted_strings_from_strmap (data.children);

  children_node.add_attribute ("is_leaf_sometimes", (data.count_as_leaf > 0) ? "1" : "0");
  for (auto const& child_name : sorted_children)
  {
    auto const& child = must_get (data.children, child_name);
    auto child_node = children_listing_node.add_child ("element");
    auto const& full_child = must_get (short_data, child_name);

    child_node.add_attribute ("name", child_name);
    set_short_child_type_attributes (data, child, full_child, child_node);
  }
  for (auto const& common_name : data.common)
  {
    auto element_in_common_node = children_common_node.add_child ("element");

    element_in_common_node.add_attribute ("name", common_name);
  }
  for (auto const& exclusive_name : data.exclusives)
  {
    auto element_in_exclusives_node = children_exclusives_node.add_child ("element");

    element_in_exclusives_node.add_attribute ("name", exclusive_name);
  }
}

// TODO: probably not needed anymore
void
build_short_parents (ShortNode const& data,
                     Xml::Node& element_node)
{
  auto parents_node = element_node.add_child ("parents");
  auto const sorted_parents = get_sorted_strings_from_strmap (data.parents);

  for (auto const& parent_name : sorted_parents)
  {
    auto parent_node = parents_node.add_child ("parent");

    parent_node.add_attribute ("name", parent_name);
  }
}

void
build_short_single_element (StrMap<ShortNode> const& short_data,
                            ShortNode const& data,
                            Xml::Node& short_node)
{
  auto element_node = short_node.add_child ("element");

  element_node.add_attribute ("name", data.name);
  build_short_attributes (data, element_node);
  build_short_children (short_data, data, element_node);
  build_short_parents (data, element_node);
}

void
build_short (Xml::Node& root,
             StrMap<ShortNode> const& short_data)
{
  auto short_node = root.add_child ("short");
  auto const sorted = get_sorted_strings_from_strmap (short_data);

  for (auto const& name : sorted)
  {
    build_short_single_element (short_data, must_get (short_data, name), short_node);
  }
}

} // anonymous namespace

// static
SchemaDoc
SchemaDoc::get_from_schema_data (SchemaData const& data)
{
  auto doc = SchemaDoc {};
  auto root = doc.doc.add_root ("lameschema");

  build_names (root, data.names);
  build_short (root, data.short_data);

  return doc;
}

void
SchemaDoc::save (std::ostream& os)
{
  doc.save (os);
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
