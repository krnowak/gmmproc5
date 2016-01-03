#ifndef MM_XML_GENERIC_CHILD_HH
#define MM_XML_GENERIC_CHILD_HH

#include "exceptions.hh"

#include <mm/utils/type-to-type.hh>
#include <mm/xml/base/xml.hh>

#include <boost/range/iterator_range_core.hpp>

#include <experimental/optional>

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Mm
{

namespace Xml
{

namespace XmlDetails
{

std::size_t
siblings_count (Base::Node const& node);

void
ensure_empty (Base::Node const& node);

void
remove_one (Base::Node const& node);

void
remove_siblings (Base::Node const& node);

class SingleChildTag;
class VectorChildTag;
class MapChildTag;
class VectorMapChildTag;
class OptionalChildTag;

template <typename ChildType>
class CommonChildImpl
{
public:
  static void
  set (ChildType& child, Base::Node& raw_child)
  {
    child.process_node (raw_child);
    ensure_empty (raw_child);
  }
};

template <typename ChildTagType>
class SingleChildImpl
{
public:
  using Tag = SingleChildTag;
  using ChildType = Utils::TypeToTypeT<ChildTagType>;
  using ContainerType = ChildType;

  static auto const&
  get (ContainerType const& child)
  {
    return child;
  }

  static void
  set (ContainerType& child, Base::Node& raw_child)
  {
    CommonChildImpl<ChildType>::set (child, raw_child);
    remove_one (raw_child);
  }
};

template <bool use_optional>
class OptionalContainer
{
public:
  template <typename ChildType>
  using Type = std::experimental::optional<ChildType>;

  template <typename ChildType>
  static void
  set (Type<ChildType>& container, Base::Node& raw_child)
  {
    ChildType child;
    CommonChildImpl<ChildType>::set (child, raw_child);
    container = std::move (child);
  }
};

template <>
class OptionalContainer<false>
{
public:
  template <typename ChildType>
  using Type = std::unique_ptr<ChildType>;

  template <typename ChildType>
  static void
  set (Type<ChildType>& container, Base::Node& raw_child)
  {
    auto tmp_container = std::make_unique<ChildType> ();
    auto& child = *(tmp_container.get ());
    CommonChildImpl<ChildType>::set (child, raw_child);
    container.swap (tmp_container);
  }
};

template <typename ChildTagType, bool use_optional>
class OptionalChildImpl
{
  using OptCont = OptionalContainer<use_optional>;

public:
  using Tag = OptionalChildTag;
  using ChildType = Utils::TypeToTypeT<ChildTagType>;
  using ContainerType = typename OptCont::template Type<ChildType>;

  static auto const&
  get (ContainerType const& opt_child)
  {
    return opt_child;
  }

  static void
  set (ContainerType& opt_child, Base::Node& raw_child)
  {
    OptCont::set (opt_child, raw_child);
    remove_one (raw_child);
  }
};

template <typename ChildTagType>
class VectorChildImpl
{
  using CType = Utils::TypeToTypeT<ChildTagType>;
  using VType = std::vector<CType>;
  using IdxType = typename VType::size_type;

public:
  using Tag = VectorChildTag;
  using ChildType = CType;
  using ContainerType = VType;

  static auto const&
  get (ContainerType const& children, IdxType idx)
  {
    return children[idx];
  }

  static IdxType
  size (ContainerType const& children)
  {
    return children.size ();
  };

  static auto
  range (ContainerType& children)
  {
    return boost::make_iterator_range (std::begin (children), std::end (children));
  }

  static auto
  range (ContainerType const& children)
  {
    return boost::make_iterator_range (std::cbegin (children), std::cend (children));
  }

  static void
  set (ContainerType& children, Base::Node& raw_child)
  {
    auto const name = raw_child.name ();

    children.clear ();
    children.reserve (siblings_count (raw_child));
    for (auto n : raw_child.siblings (name))
    {
      ChildType child;

      CommonChildImpl<ChildType>::set (child, n);
      children.push_back (std::move (child));
    }
    remove_siblings (raw_child);
  }
};

template <typename ChildTagType, typename Name>
class MapChildBaseImpl
{
  using CType = Utils::TypeToTypeT<ChildTagType>;

  static_assert (CType::template HasStringAttribute<Name>::value, "child has required key string attribute");

  using MType = std::unordered_map<std::string, CType>;
  using KeyType = typename MType::key_type;

public:
  using Tag = MapChildTag;
  using ChildType = CType;
  using ContainerType = MType;

  static ChildType const*
  get (ContainerType const& children, KeyType const& name)
  {
    auto iter = children.find (name);
    if (iter != std::end (children))
    {
      return &iter->second;
    }
    return nullptr;
  }

  static void
  set (ContainerType& children, Base::Node& raw_child)
  {
    auto child_name = raw_child.name ();

    children.clear ();
    children.reserve (siblings_count (raw_child));

    for (auto n : raw_child.siblings (child_name))
    {
      ChildType child;
      auto const name_attr = n.attribute (Name::raw.data ());

      if (!name_attr)
      {
        throw InvalidParserAssumption (Attrs, n, Name::raw.data ());
      }
      auto const name = name_attr.value ();

      if (children.find (name) != std::cend (children))
      {
        // what exception to throw here?
        throw std::runtime_error ("Expected child with unique name");
      }

      CommonChildImpl<ChildType>::set (child, n);
      children.insert ({name, std::move (child)});
    }
    remove_siblings (raw_child);
  }
};

template <typename ChildTagType, typename Name>
class VectorMapChildBaseImpl
{
  using CType = Utils::TypeToTypeT<ChildTagType>;

  static_assert (CType::template HasStringAttribute<Name>::value, "child has required key string attribute");

  using Ref = std::reference_wrapper<CType>;
  using VType = std::vector<CType>;
  using IdxType = typename VType::size_type;
  using MType = std::unordered_map<std::string, Ref>;
  using KeyType = typename MType::key_type;
  using TupleType = std::tuple<VType, MType>;

public:
  using Tag = VectorMapChildTag;
  using ChildType = CType;
  using ContainerType = TupleType;

  static auto const&
  get (ContainerType const& children, IdxType idx)
  {
    return std::get<VType> (children)[idx];
  }

  static IdxType
  size (ContainerType const& children)
  {
    return std::get<VType> (children).size ();
  };

  static ChildType const*
  get (ContainerType const& children, KeyType const& key)
  {
    auto const& map_children = std::get<MType> (children);
    auto iter = map_children.find (key);

    if (iter != std::cend (map_children))
    {
      return &iter->second.get ();
    }
    return nullptr;
  }

  static auto
  range (ContainerType& children)
  {
    auto& vector_children = std::get<VType> (children);
    return boost::make_iterator_range (std::begin (vector_children), std::end (vector_children));
  }

  static auto
  range (ContainerType const& children)
  {
    auto const& vector_children = std::get<VType> (children);
    return boost::make_iterator_range (std::cbegin (vector_children), std::cend (vector_children));
  }

  static void
  set (ContainerType& children, Base::Node& raw_child)
  {
    auto& vector_children = std::get<VType> (children);
    std::unordered_set<std::string> names;
    auto child_name = raw_child.name ();

    vector_children.clear ();
    vector_children.reserve (siblings_count (raw_child));
    names.reserve (vector_children.size ());
    for (auto n : raw_child.siblings (child_name))
    {
      ChildType child;
      auto const name_attr = n.attribute (Name::raw.data ());

      if (!name_attr)
      {
        throw InvalidParserAssumption (Attrs, n, Name::raw.data ());
      }
      std::string const name {name_attr->value ()};

      if (names.find (name) != std::cend (names))
      {
        // TODO: what exception to throw here?
        throw std::runtime_error ("Expected child with unique name " + name);
      }

      names.insert (name);
      CommonChildImpl<ChildType>::set (child, n);
      vector_children.push_back (std::move (child));
    }

    auto& map_children = std::get<MType> (children);

    // Map population must happen after vector is completely
    // populated, so nothing more is added to the vector, so no
    // reference will be invalidated.
    map_children.clear ();
    map_children.reserve (vector_children.size ());

    for (auto& child : vector_children)
    {
      map_children.insert ({child.template get<Name> (), std::ref (child)});
    }
    remove_siblings (raw_child);
  }
};

template <typename Name>
class GetKeyedChildImpl
{
public:
  template <typename ChildTagType>
  using MapType = MapChildBaseImpl<ChildTagType, Name>;

  template <typename ChildTagType>
  using VectorMapType = VectorMapChildBaseImpl<ChildTagType, Name>;
};

template <bool use_optional>
class GetOptionalChildImpl
{
public:
  template <typename ChildTagType>
  using Type = OptionalChildImpl<ChildTagType, use_optional>;
};

} // namespace XmlDetails

template <typename NameString, typename ChildTagType, template <typename> class ChildImpl>
class GenericChild
{
  //static_assert (Utils::HasToType<ChildTagType>::value, "ChildTagType has an associated ChildType");

public:
  using Name = NameString;
  using TagType = ChildTagType;
  using ImplType = ChildImpl<TagType>;
  using ImplTag = typename ImplType::Tag;

  static void
  set (typename ImplType::ContainerType& child, Base::Node& node)
  {
    auto raw_child = node.child (Name::raw.data ());
    if (raw_child)
    {
      ImplType::set (child, *raw_child);
    }
  }
};

// some standard attribute aliases

template <typename NameString, typename ChildTagType>
using SingleChild = GenericChild<NameString, ChildTagType, XmlDetails::SingleChildImpl>;

template <typename NameString, typename ChildTagType>
using VectorChild = GenericChild<NameString, ChildTagType, XmlDetails::VectorChildImpl>;

template <typename NameString, typename ChildTagType, typename Name>
using MapChild = GenericChild<NameString, ChildTagType, XmlDetails::GetKeyedChildImpl<Name>::template MapType>;

template <typename NameString, typename ChildTagType, typename Name>
using VectorMapChild = GenericChild<NameString, ChildTagType, XmlDetails::GetKeyedChildImpl<Name>::template VectorMapType>;

using UseStdOptional = std::true_type;
using UseStdUniquePtr = std::false_type;

template <typename NameString, typename ChildTagType, typename WhatToUse>
using OptionalChild = GenericChild<NameString, ChildTagType, XmlDetails::GetOptionalChildImpl<WhatToUse::value>::template Type>;

} // namespace Xml

} // namespace Mm

#endif // MM_XML_GENERIC_CHILD_HH
