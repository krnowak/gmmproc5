#ifndef GMMPROC_XML_STRUCTURED_API_HH
#define GMMPROC_XML_STRUCTURED_API_HH

#include <boost/hana/type.hpp>

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

// part API

class GetType
{};

template <typename PartTagP,
          typename PartTypeP>
constexpr auto
get_type (PartTagP tag,
          PartTypeP part_type)
{
  return NotOverridden {GetType {},
                        tag,
                        part_type};
}

class GetAccessInfo
{};

template <typename PartTagP,
          typename PartTypeP>
constexpr auto
get_access_info (PartTagP tag,
                 PartTypeP part_type)
{
  return NotOverridden {GetAccessInfo {},
                        tag,
                        part_type};
}

class GetPartKind
{};

template <typename PartTagP>
constexpr auto
get_part_kind (PartTagP)
{
  return PartKind::Normal;
}

class GeneratePartMember
{};

template <typename PartTagP,
          typename PartTypeP,
          typename StructuredTagTypeP,
          typename... InputP>
decltype(auto)
generate_part_member (PartTagP tag,
                      PartTypeP part_type,
                      StructuredTagTypeP structured_tag_type,
                      InputP&&... input)
{
  return NotOverridden {GeneratePartMember {},
                        tag,
                        part_type,
                        structured_tag_type,
                        std::forward<InputP> (input)...};
}

// getter API

class GetGetterType
{};

template <typename GetterTagP,
          typename ContainerInfoTypeP>
constexpr auto
get_getter_type (GetterTagP getter_tag,
                 ContainerInfoTypeP container_info_type)
{
  return NotOverridden {GetGetterType {},
                        getter_tag,
                        container_info_type};
}

// child API

class GetChildType
{};

template <typename BasicP,
          typename RealTypeP>
constexpr auto
get_child_type (BasicP basic,
                RealTypeP real_type)
{
  return NotOverridden {GetChildType {},
                        basic,
                        real_type};
}

class GetChildGetterTagsAndPolicies
{};

template <typename BasicP>
constexpr auto
get_child_getter_tags_and_policies (BasicP)
{
  return NotOverridden {GetChildGetterTagsAndPolicies {},
                        basic};
}

class GenerateChildPartMember
{};

template <typename BasicP,
          typename KindTypeP,
          typename StructuredTagTypeP,
          typename... InputP>
decltype(auto)
generate_child_part_member (BasicP basic,
                            KindTypeP kind_type,
                            StructuredTagTypeP structured_tag_type,
                            Input&&... input)
{
  return NotOverridden {GenerateChildPartMember {},
                        basic,
                        structured_tag_type,
                        kind_type,
                        std::forward<InputP> (input)...};
}

// kind API

class GetKindType
{};

template <typename KindTagP,
          typename KindTypeP>
constexpr auto
get_kind_type (KindTagP kind_tag,
               KindTypeP kind_type)
{
  return NotOverridden {GetKindType {},
                        kind_tag,
                        kind_type};
}

class GenerateKindPartMember
{};

template <typename KindTagP,
          typename KindTypeP,
          typename StructuredTagType,
          typename... InputP>
decltype(auto)
generate_kind_part_member (KindTagP kind_tag,
                           KindTypeP kind_type,
                           StructuredTagTypeP structured_tag_type,
                           Input&&... input)
{
  return NotOverridden {GenerateKindPartMember {},
                        kind_tag,
                        kind_type,
                        structured_tag_type,
                        std::forward<InputP> (input)...};
}

// primitive API

class GetRealPrimitiveType
{};

template <typename PrimitiveTagP,
          typename PrimitiveP>
constexpr auto
get_real_primitive_type (PrimitiveTagP primitive_tag,
                         PrimitiveP primitive)
{
  return NotOverridden {GetRealPrimitiveType {},
                        primitive_tag,
                        primitive};
}

class GetPrimitive
{};

template <typename StructuredTagP,
          typename RealPrimitiveTypeP,
          typename... InputP>
decltype(auto)
get_primitive (StructuredTagP structured_tag,
               RealPrimitiveTypeP real_primitive_type,
               Input&&... input)
{
  return NotOverridden {GetPrimitive {},
                        structured_tag,
                        real_primitive_type,
                        std::forward<Input> (input)...};
}

// node API

class GetNodeType
{};

template <typename NodeTagP>
constexpr auto
get_node_type (NodeTagP node_tag)
{
  return NotOverridden {GetNodeType {}, node_tag};
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
                   boost::hana::make_type (part));
}

template <typename PartP>
constexpr auto
get_access_info (PartP part)
{
  using API::get_access_info;

  return get_access_info (part.tag,
                          boost::hana::make_type (part));
}

template <typename PartP>
constexpr auto
get_part_kind (PartP part)
{
  using API::get_part_kind;

  return get_part_kind (part.tag);
}

// TODO: somehow wrap the input parameters, so they don't participate in ADL?
template <typename PartP,
          typename StructuredTagP,
          typename... InputP>
decltype(auto)
generate_part_member (PartP part,
                      StructuredTagP structured_tag,
                      InputP&&... input)
{
  using API::generate_part_member;

  return generate_part_member (part.tag,
                               NoADL::wrap (part),
                               NoADL::wrap (structured_tag),
                               std::forward<InputP> (input)...);
}

// getter API

template <typename GetterTagP,
          typename ContainerInfoP>
constexpr auto
get_getter_type (GetterTagP getter_tag,
                 ContainerInfoP container_info)
{
  using API::get_getter_type;

  return get_getter_type (getter_tag,
                          boost::hana::make_type (container_info));
}

// child API

template <typename ChildP,
          typename RealTypeP>
constexpr auto
get_child_type (ChildP child,
                RealTypeP real_type)
{
  using API::get_child_type;

  return get_child_type (child.basic,
                         real_type);
}

template <typename ChildP>
constexpr auto
get_child_getter_tags_and_policies (ChildP child)
{
  using API::get_child_getter_tags_and_policies;

  return get_child_getter_tags_and_policies (child.basic);
}

template <typename ChildP,
          typename StructuredTagType,
          typename... InputP>
decltype(auto)
generate_child_part_member (ChildP child,
                            StructuredTagType structured_tag_type,
                            InputP... input)
{
  using API::generate_child_part_member;

  return generate_child_part_member (child.basic,
                                     child.kind,
                                     structured_tag_type,
                                     std::forward<Input> (input)...);
}

// kind API

template <typename KindP>
constexpr auto
get_kind_type (KindP kind)
{
  using API::get_kind_type;

  return get_kind_type (kind.tag,
                        NoADL::wrap (kind));
}

template <typename KindP,
          typename StructuredTagType,
          typename... InputP>
decltype(auto)
generate_kind_part_member (KindP kind,
                           StructuredTagType structured_tag_type,
                           InputP&&... input)
{
  using API::generate_kind_part_member;

  return generate_kind_part_member (kind.tag,
                                    NoADL::wrap(kind),
                                    structured_tag_type,
                                    std::forward<InputP> (input)...);
}

// primitive API

template <typename PrimitiveP>
constexpr auto
get_real_primitive_type (PrimitiveP primitive)
{
  using API::get_real_primitive_type;

  return get_real_primitive_type (primitive.tag,
                                  primitive);
}

template <typename StructuredTagP,
          typename RealPrimitiveTypeP,
          typename... InputP>
decltype(auto)
get_primitive (StructuredTagP structured_tag,
               RealPrimitiveTypeP real_primitive_type,
               Input&&... input)
{
  using API::get_primitive;

  return get_primitive (structured_tag,
                        real_primitive_type,
                        std::forward<Input> input...);
}

// node API

template <typename NodeTagP>
constexpr auto
get_node_type (NodeTagP node_tag)
{
  using API::get_node_type;

  return get_node_type (node_tag);
}

} // namespace Convenience

} // namespace Gmmproc::Xml::Structured::API

#endif // GMMPROC_XML_STRUCTURED_API_HH
