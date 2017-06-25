#ifndef GMMPROC_XML_STRUCTURED_CHILD_HH
#define GMMPROC_XML_STRUCTURED_CHILD_HH

#include <gmmproc/xml/structured/detail/utils.hh>

#include <gmmproc/xml/structured/api.hh>
#include <gmmproc/xml/structured/getters.hh>
#include <gmmproc/xml/structured/part.hh>

#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>

namespace Gmmproc::Xml::Structured
{

class NodeKindTag
{};

template <typename NodeTagP>
class NodeKind
{
public:
  using Tag = NodeKindTag;
  using NodeTag = NodeTag;

  Tag tag;
  NodeTag node_tag;
};

template <typename NodeKindTypeP>
constexpr auto
get_kind_type (NodeKindTag,
               NodeKindTypeP node_kind_type)
{
  auto node_kind {boost::hana::traits::declval (node_tag_type)};

  return API::Convenience::get_node_type (node_kind.node_tag);
}

template <typename NodeKindTypeP,
          typename StructuredTagType,
          typename... InputP>
decltype(auto)
generate_kind_part_member (NodeKindTag node_kind_tag,
                           NodeKindTypeP node_kind_type,
                           StructuredTagTypeP structured_tag_type,
                           Input&&... input)
{
  // TODO
}

class ValueKindTag
{};

template <typename PrimitiveP>
class ValueKind
{
public:
  using Tag = ValueKindTag;
  using Primitive = PrimitiveP;

  Tag tag;
  Primitive primitive;
};

template <typename ValueKindTypeP>
constexpr auto
get_kind_type (ValueKindTag,
               ValueKindTypeP value_kind_type)
{
  auto value_kind {boost::hana::traits::declval (value_kind_type)};

  return API::Convenience::get_real_primitive_type (value_kind.primitive);
}

template <typename ValueKindTypeP,
          typename StructuredTagType,
          typename... InputP>
decltype(auto)
generate_kind_part_member (ValueKindTag value_kind_tag,
                           ValueKindTypeP value_kind_type,
                           StructuredTagTypeP structured_tag_type,
                           Input&&... input)
{
  constexpr auto value_kind {NoADL::unwrap (value_kind_type)};
  constexpr auto real_type {API::Convenience::get_real_primitive_type (value_kind.primitive)};
  auto structured_tag {NoADL::unwrap (structured_tag_type)};

  return API::Convenience::get_primitive (structured_tag,
                                          real_type,
                                          std::forward<Input> (input)...);
}

class ChildTag
{};

template <typename AccessKeyP,
          typename BasicP,
          typename KindP>
class Child : public Part<ChildTag>
{
public:
  using AccessKey = AccessKeyP;
  using Basic = BasicP;
  using Kind = KindP;

  AccessKey access_key;
  Basic basic;
  Kind kind;
};

template <typename ChildTypeP>
constexpr auto
get_type (ChildTag,
          ChildTypeP child_type)
{
  auto child {boost::hana::traits::declval (child_type)};
  auto real_type {API::Convenience::get_kind_type (child.kind)};

  return API::Convenience::get_child_type (child,
                                           real_type);
}

template <typename ChildTypeP>
constexpr auto
get_access_info (ChildTag,
                 ChildTypeP child_type)
{
  auto child {boost::hana::traits::declval (child_type)};
  auto getters {API::Convenience::get_child_getter_tags_and_policies (child)};
  auto access_info_pair {boost::hana::make_pair (child.access_key,
                                                 getters)};

  return Detail::make_tuple_and_map (boost::hana::make_tuple (access_info_pair));
}

template <typename ChildTypeP,
          typename StructuredTagTypeP,
          typename... InputP>
decltype(auto)
generate_part_member (ChildTag,
                      ChildTypeP child_type,
                      StructuredTagTypeP structured_tag_type,
                      InputP&&... input)
{
  constexpr auto child {NoADL::unwrap (child_type)};

  return API::Convenience::generate_child_part_member (child,
                                                       structured_tag_type,
                                                       std::forward<Input> (input)...);
}

} // namespace Gmmproc::Xml::Structured

#endif // GMMPROC_XML_STRUCTURED_CHILD_HH
