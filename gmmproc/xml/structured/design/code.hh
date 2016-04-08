namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Basic
{

class Single
{
  // TODO
};

template <typename OptionalTypeP> // ptr or val
class Optional
{
  // TODO
};

class Vector
{
  // TODO
};

template <typename AttrKeysP> // mpl::vector<StaticString<"name">, StaticString<"c:type">>
class MapVector
{
  // TODO
};

template <typename TypeP, typename TagP> // TypeP - single, optional, vector, mapvector
class Child
{
  // TODO
};

} // namespace Basic

namespace Ext
{

template <typename AliasP, typename TypesAndTagsP> // mpl::vector<std::pair<TypeP, TagP>, ...>
class Exclusive
{
  // TODO
};

template <typename AliasP, typename OptionalTypeP, typename TypesAndTagsP>
class OptExclusive
{
  // TODO
};

} // namespace Ext

namespace Attribute
{

class String
{
  // TODO
};

class Int
{
  // TODO
};

class OptBool
{
  // TODO
}

template <bool DefaultVP>
class Bool
{
  // TODO
};

} // namespace Attribute

namespace Registry
{

template <typename AttributeP>
class AttrInfo;

template <typename NodeP>
class NodeInfo;

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
    Sxml::Ext::OptVariant<
      TypeInfo,
      Sxml::Basic::OptPtr,
      mpl::vector<
        std::pair<Sxml::Single, Tag::Array>,
        std::pair<Sxml::Vector, Tag::Type>
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
        std::pair<Sxml::Single, Tag::Array>,
        std::pair<Sxml::Single, Tag::Type>
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
    Attribute::FixedSize,
    Attribute::Length,
    Attribute::ZeroTerminated,

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
