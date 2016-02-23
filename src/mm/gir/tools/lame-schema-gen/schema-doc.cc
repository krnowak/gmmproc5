#include "schema-doc.hh"
#include "schema-data.hh"
#include "types.hh"
#include "utils.hh"

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
build_names (Xml::Base::Node& root,
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

// optional-ptr - bottommost recursive node (type -> array -> type)
//                                                opt      ptr
// optional-opt - others
void
set_short_child_type_attributes (ShortNode::Child const& child,
                                 ShortNode const& full_child,
                                 Xml::Base::Node& child_node)
{
  switch (get_child_type (child))
  {
  case ChildType::SINGLE:
    child_node.add_attribute ("type", "single");
    break;

  case ChildType::OPTIONAL:
    child_node.add_attribute ("type", "optional");
    // TODO: use opt for non-recursive children
    child_node.add_attribute ("extra-type-info", "ptr");
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
        child_node.add_attribute ("type", "vector-map");
        child_node.add_attribute ("extra-type-info", boost::algorithm::join (unique_attrs, "|"));
      }
    }
    break;
  }
}

void
build_short (Xml::Base::Node& root,
             StrMap<ShortNode> const& short_data)
{
  auto short_node = root.add_child ("short");
  auto const sorted = get_sorted_strings_from_strmap (short_data);

  for (auto const& name : sorted)
  {
    auto const& data = must_get (short_data, name);
    auto element_node = short_node.add_child ("element");
    auto attributes_node = element_node.add_child ("attributes");
    auto children_node = element_node.add_child ("children");
    auto parents_node = element_node.add_child ("parents");
    auto const sorted_attrs = get_sorted_strings_from_strmap (data.attributes);
    auto const sorted_children = get_sorted_strings_from_strmap (data.children);
    auto const sorted_parents = get_sorted_strings_from_strmap (data.parents);

    element_node.add_attribute ("name", name);
    for (auto const& attr_name : sorted_attrs)
    {
      auto const& attr = must_get (data.attributes, attr_name);
      auto attribute_node = attributes_node.add_child ("attribute");

      attribute_node.add_attribute ("name", attr_name);
      attribute_node.add_attribute ("type", attr.to_string ());
    }
    for (auto const& child_name : sorted_children)
    {
      auto const& child = must_get (data.children, child_name);
      auto child_node = children_node.add_child ("element");
      auto const& full_child = must_get (short_data, child_name);

      child_node.add_attribute ("name", child_name);
      set_short_child_type_attributes (child, full_child, child_node);
    }
    for (auto const& parent_name : sorted_parents)
    {
      auto parent_node = parents_node.add_child ("parent");
      parent_node.add_attribute ("name", parent_name);
      parent_node.add_attribute ("recursion_point", must_get (data.parents, parent_name) ? "1" : "0");
    }
  }
}

void
set_long_child_type_attributes (LongNode const& child,
                                Xml::Base::Node& child_node)
{
  switch (get_child_type (child))
  {
  case ChildType::SINGLE:
    child_node.add_attribute ("type", "single");
    break;

  case ChildType::OPTIONAL:
    child_node.add_attribute ("type", "optional");
    child_node.add_attribute ("extra-type-info", "opt");
    break;

  case ChildType::MULTI:
    {
      auto unique_attrs = get_unique_attribute_names (child.attributes);
      if (unique_attrs.empty ())
      {
        child_node.add_attribute ("type", "vector");
      }
      else
      {
        child_node.add_attribute ("type", "vector-map");
        child_node.add_attribute ("extra-type-info", boost::algorithm::join (unique_attrs, "|"));
      }
    }
    break;
  }
}

void
build_long (Xml::Base::Node& root,
            std::unique_ptr<LongNode> const& toplevel_long_node)
{
  using NodeRef = std::reference_wrapper<LongNode const>;
  using NodePair = std::pair<Xml::Base::Node, NodeRef>;
  auto long_node = root.add_child ("long");
  std::queue<NodePair> nodes;

  nodes.emplace (long_node, *toplevel_long_node);
  while (!nodes.empty ())
  {
    auto node_pair = nodes.front ();
    auto& parent = node_pair.first;
    auto const& lnode = node_pair.second.get ();
    auto element_node = parent.add_child ("element");
    auto attributes_node = element_node.add_child ("attributes");
    auto children_node = element_node.add_child ("children");
    auto const sorted_attributes = get_sorted_strings_from_strmap (lnode.attributes);
    auto const sorted_children = get_sorted_strings_from_strmap (lnode.children);

    nodes.pop ();
    element_node.add_attribute ("name", lnode.name);
    set_long_child_type_attributes (lnode, element_node);
    for (auto const& attr_name : sorted_attributes)
    {
      auto attribute_node = attributes_node.add_child ("attribute");
      auto const& attr = must_get (lnode.attributes, attr_name);

      attribute_node.add_attribute ("name", attr_name);
      attribute_node.add_attribute ("type", attr.to_string ());
    }
    for (auto const& child_name : sorted_children)
    {
      nodes.emplace (children_node, *must_get (lnode.children, child_name));
    }
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
  build_long (root, data.toplevel_long_node);

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
