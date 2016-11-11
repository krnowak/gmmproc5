#include <boost/variant/variant.hpp>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

// child types (Basic::Child, Ext::{Opt,}Variant), export
// GetContainedType metafunction

namespace Basic
{

class Single
{
  class GetContainedType
  {
  public:
    template <typename TagP>
    class apply
    {
    public:
      using type = typename Registry::NodeInfo<TagP>::Type;
    };
  };
  // TODO
};

enum class OptionalType : int
{
  Pointer,
  Value
};

template <OptionalType OptionalTypeP> // ptr or val
class Optional
{
  class GetContainedType
  {
  public:
    template <typename TagP>
    class apply
    {
    private:
      using NodeType = typename Registry::NodeInfo<TagP>::Type;

    public:
      using type = Detail::GetOptionalTypeT<NodeType, OptionalTypeP>;
    };
  };
  // TODO
};

class Vector
{
  class GetContainedType
  {
  public:
    template <typename TagP>
    class apply
    {
    private:
      using NodeType = typename Registry::NodeInfo<TagP>::Type;

    public:
      using type = std::vector<NodeType>;
    };
  };
  // TODO
};

template <typename AttrKeysP> // mpl::vector<StaticString<"name">, StaticString<"c:type">>
class MapVector
{
  class GetContainedType
  {
  public:
    template <typename TagP>
    class apply
    {
    private:
      using NodeType = typename Registry::NodeInfo<TagP>::Type;
      using VectorType = std::vector<NodeType>;
      using MapType = std::unordered_map<std::string, std::reference_wrapper<NodeType>>;
      using Range = boost::mpl::range_c<int, 0, boost::mpl::size<AttrKeysP>::type::value>;
      using FinalVector = typename boost::mpl::fold<Range,
                                                    boost::mpl::vector<VectorType>,
                                                    boost::mpl::push_back<_1, MapType>
                                                   >::type;

    public:
      using type = GetStdTupleTypeT<FinalVector>;
    };
  };
  // TODO
};

class ChildTag;

template <typename TypeP, typename NodeTagP> // TypeP - single, optional, vector, mapvector
class Child
{
public:
  class GetContainedType
  {
  public:
    class apply
    {
    public:
      using type = typename TypeP::GetContainedType::apply<NodeTagP>::type;
    };
  };
  using Tag = ChildTag;
  // TODO
};

class ChildImpl
{
public:
  template <typename ChildP> // ChildP is Child<TypeP, NodeTagP>
  class apply
  {

  };
};

} // namespace Basic

namespace Ext
{

class ExclusiveTag;
class OptExclusiveTag;

template <typename AliasP, typename TypesAndTagsP> // mpl::vector<mpl::pair<TypeP, TagP>, ...>
class Exclusive
{
public:
  class GetContainedType
  {
  public:
    class apply
    {
    public:
      using type = typename boost::make_variant_over<GetContainedTypesT<TypesAndTagsP>>::type;
    };
  };
  using Tag = ExclusiveTag;
  // TODO
};

class ExclusiveImpl
{
public:
  template <typename ExclusiveP>
  class apply
  {

  };
};

template <typename AliasP, typename OptionalTypeP, typename TypesAndTagsP>
class OptExclusive
{
  class GetContainedType
  {
  public:
    class apply
    {
    private:
      using Variant = typename boost::make_variant_over<GetContainedTypesT<TypesAndTagsP>>::type;
    public:
      using type = Detail::GetOptionalTypeT<Variant, OptionalTypeP>;
    };
  };
  using Tag = OptExclusiveTag;
  // TODO
};

class OptExclusiveImpl
{
public:
  template <typename ExclusiveP>
  class apply
  {

  };
};

} // namespace Ext

namespace Registry
{

template<>
class ChildInfo<Basic::ChildTag>
{
  using ImplMetaFunc = Basic::ChildImpl;
};

template<>
class ChildInfo<Ext::ExclusiveTag>
{
  using ImplMetaFunc = Ext::ExclusiveImpl;
};

template<>
class ChildInfo<Ext::OptExclusiveTag>
{
  using ImplMetaFunc = Ext::OptExclusiveImpl;
};

} // namespace Registry

namespace Attribute
{

// export Type (std::string, bool, int, ...)

class String
{
public:
  using Type = std::string;
  // TODO
};

class Int
{
public:
  using Type = int;
  // TODO
};

class OptBool
{
public:
  using Type = std::experimental::optional<bool>;
  // TODO
}

template <bool DefaultValueP>
class Bool
{
public:
  using Type = bool;
  // TODO
};

} // namespace Attribute

namespace Registry
{

// AttrInfo, export Basic (Attribute::{String,Int,OptBool,Bool<...>})

template <typename AttributeKeyP>
class AttrInfo;

// NodeInfo, export

template <typename NodeTagP>
class NodeInfo;

// ChildInfo

template <typename ChildTagP>
class ChildInfo;

} // namespace Registry

template <typename AttrListP, typename ChildListP, typename HasTextTypeP>
class Node
{
  // TODO
};

template <typename ChildTagP>
using DocNode = Node<
  mpl::vector<
    // no attributes
  >,
  mpl::vector<
    Basic::Child<Basic::Single, ChildTagP>
  >
>;

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

namespace Gmmproc
{

namespace Gir
{

namespace Tag
{

class Array;
class Type;

} // namespace Tag

namespace Attribute
{

class Name;
class CType;

} // namespace Attribute

namespace Sxml = Xml::Structured;

// aliases for Exts
class TypeInfo;

using Type = Sxml::Node<
  mpl::vector<
    Attribute::Name,
    Attribute::CType,
  >,
  mpl::vector<
    Sxml::Basic::Child<Sxml::Basic::Optional<Sxml::Basic::OptVal>, DocTag>,
    Sxml::Ext::OptVariant<
      TypeInfo,
      Sxml::Basic::OptPtr,
      mpl::vector<
        boost::mpl::pair<Sxml::Single, Tag::Array>,
        boost::mpl::pair<Sxml::Vector, Tag::Type>
      >
    >
  >
>;

using Array = Sxml::Node<
  mpl::vector<
    Attribute::CType,
    Attribute::FixedSize,
    Attribute::Length,
    Attribute::Name,
    Attribute::ZeroTerminated,
  >,
  mpl::vector<
    Sxml::Ext::Variant<
      TypeInfo,
      Sxml::Basic::OptPtr,
      mpl::vector<
        boost::mpl::pair<Sxml::Single, Tag::Array>,
        boost::mpl::pair<Sxml::Single, Tag::Type>
      >
    >
  >
>;

} // namespace Gir

namespace Xml
{

namespace Structured
{


namespace Registry
{

// attribute registration

template <>
class AttrInfo<Gir::Attribute::Name>
{
public:
  using Name = Gir::Names::Name;
  using Type = Attribute::String;
};

template <>
class AttrInfo<Gir::Attribute::CType>
{
public:
  using Name = Gir::Names::CType;
  using Type = Attribute::String;
};

template <>
class AttrInfo<Gir::Attribute::FixedSize>
{
public:
  using Name = Gir::Names::FixedSize;
  using Type = Attribute::Int;
};

template <>
class AttrInfo<Gir::Attribute::Length>
{
public:
  using Name = Gir::Names::Length;
  using Type = Attribute::Int;
};

template <>
class AttrInfo<Gir::Attribute::ZeroTerminated>
{
public:
  using Name = Gir::Names::ZeroTerminated;
  using Type = Attribute::OptBool;
};

// node registration

template <>
class NodeInfo<Gir::Tag::Array>
{
public:
  using Name = Gir::Names::Array;
  using Type = Gir::Array;
};

template <>
class NodeInfo<Gir::Tag::Type>
{
public:
  using Name = Gir::Names::Type;
  using Type = Gir::Type;
};

} // namespace Registry

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc
