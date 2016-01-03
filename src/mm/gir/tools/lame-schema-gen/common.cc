#include "common.hh"

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

// prereq: first != last
template <typename Iterator>
bool
is_numeric_range (Iterator first, Iterator const &last)
{
  if (*first == '-')
  {
    ++first;
  }
  if (first == last)
  {
    return false;
  }
  return std::find_if (first, last, [](auto c) { return !std::isdigit (c); }) == last;
}

bool
is_string_bool_value (Str const& value)
{
  return ((value == "t") || (value == "y") || (value == "true") || (value == "yes") ||
          (value == "f") || (value == "n") || (value == "false") || (value == "no"));
}

std::pair<bool, bool>
is_numeric_bool_value (Str const& value)
{
  if (value == "0")
  {
    return std::make_pair (true, false);
  }
  if (value == "1")
  {
    return std::make_pair (true, true);
  }
  return std::make_pair (false, false);
}

// prereq: value.empty () == false
AttributeType
get_attribute_type (Str const& value)
{
  if (is_numeric_range (value.cbegin (), value.cend ()))
  {
    return (is_numeric_bool_value (value) ?
            AttributeType::BOOLEAN :
            AttributeType::NUMERIC);
  }
  return (is_string_bool_value (value) ?
          AttributeType::BOOLEAN :
          AttributeType::STRING);
}

bool
is_a_leaf (Xml::Base::Node const& node)
{
  for (auto n : node.children ())
  {
    if (n.is_tag ())
    {
      return false;
    }
  }
  return true;
}

} // anonymous namespace

AttributeType
update_attribute_type (Str const& value, AttributeType const& previous_type)
{
  switch (previous_type)
  {
  case AttributeType::UNDETERMINED:
  case AttributeType::BOOLEAN:
    if (value.empty ())
    {
      return AttributeType::BOOLEAN;
    }
    return get_attribute_type (value);

  case AttributeType::NUMERIC:
    if (value.empty ())
    {
      return AttributeType::STRING;
    }
    if (!is_numeric_range (value.cbegin (), value.cend ()))
    {
      return AttributeType::STRING;
    }
    return AttributeType::NUMERIC;

  case AttributeType::STRING:
    return AttributeType::STRING;
  }

  // invalid case
  return AttributeType::UNDETERMINED;
}

LeafType
update_leaf_type (Xml::Base::Node const& node, LeafType const& previous_type)
{
  switch (previous_type)
  {
  case LeafType::UNDETERMINED:
    return (is_a_leaf (node) ? LeafType::ALWAYS_A_LEAF : LeafType::NEVER_A_LEAF);

  case LeafType::NEVER_A_LEAF:
    return (is_a_leaf (node) ? LeafType::SOMETIMES_A_LEAF : LeafType::NEVER_A_LEAF);

  case LeafType::SOMETIMES_A_LEAF:
    return LeafType::SOMETIMES_A_LEAF;

  case LeafType::ALWAYS_A_LEAF:
    return (is_a_leaf (node) ? LeafType::ALWAYS_A_LEAF : LeafType::SOMETIMES_A_LEAF);
  }

  // invalid case
  return LeafType::UNDETERMINED;
}

} // namespace LameSchemaGen

} // namespace Tools

} // namespace Gir

} // namespace Mm
