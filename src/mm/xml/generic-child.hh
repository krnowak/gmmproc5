#ifndef MM_XML_GENERIC_CHILD_HH
#define MM_XML_GENERIC_CHILD_HH

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <experimental/optional>

#include <ext/kr/kr.hh>
#include <mm/utils/range.hh>

#include "base/xml.hh"
#include "exceptions.hh"

namespace Mm
{

namespace Xml
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

template <typename ChildType>
class SingleChildImpl
{
public:
  using Tag = SingleChildTag;
  using ContainerType = ChildType;

  // we usually use ContainerType, but for this class we use ChildType
  // for clarity, since ContainerType = ChildType.
  static ChildType
  get (ChildType const& child)
  {
    return child;
  }

  static void
  set (ChildType& child, Base::Node& raw_child)
  {
    CommonChildImpl<ChildType>::set (child, raw_child);
    remove_one (raw_child);
  }
};

template <typename ChildType>
class VectorChildImpl
{
  using VType = std::vector<ChildType>;
  using IdxType = typename VType::size_type;

public:
  using Tag = VectorChildTag;
  using ContainerType = VType;

  static ChildType
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
    return Utils::Range<typename VType::iterator> (std::begin (children), std::end (children));
  }

  static auto
  range (ContainerType const& children)
  {
    return Utils::Range<typename VType::const_iterator> (std::cbegin (children), std::cend (children));
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

      CommonChildImpl<ChildType>::set (child, raw_child);
      children.push_back (std::move (child));
    }
    remove_siblings (raw_child);
  }
};

template <typename ChildType, typename Name>
class MapChildBaseImpl
{
  static_assert (ChildType::template HasStringAttribute<Name>::value, "child has required key string attribute");

  using MType = std::unordered_map<std::string, ChildType>;
  using KeyType = typename MType::key_type;

public:
  using Tag = MapChildTag;
  using ContainerType = MType;

  static auto
  get (ContainerType const& children, KeyType const& name)
  {
    auto iter = children.find (name);
    if (iter != std::end (children))
    {
      return std::experimental::make_optional (iter->second);
    }
    return std::experimental::optional<ChildType> {};
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
      auto const name_attr = n.attribute(Name::raw.data ());

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

template <typename ChildType, typename Name>
class VectorMapChildBaseImpl
{
  static_assert (ChildType::template HasStringAttribute<Name>::value, "child has required key string attribute");

  using Ref = std::reference_wrapper<ChildType>;
  using VType = std::vector<ChildType>;
  using IdxType = typename VType::size_type;
  using MType = std::unordered_map<std::string, Ref>;
  using KeyType = typename MType::key_type;
  using ListType = Kr::TypeList<VType, MType>;
  using TupleType = typename Kr::ToStdTuple<ListType>::Type;

public:
  using Tag = VectorMapChildTag;
  using ContainerType = TupleType;

  static ChildType
  get (ContainerType const& children, IdxType idx)
  {
    return std::get<VType>(children)[idx];
  }

  static IdxType
  size (ContainerType const& children)
  {
    return std::get<VType>(children).size ();
  };

  static auto
  get (ContainerType const& children, KeyType const& key)
  {
    auto const& map_children = std::get<MType>(children);
    auto iter = map_children.find (key);

    if (iter != std::cend (map_children))
    {
      return std::experimental::make_optional (iter->second.get ());
    }
    return std::experimental::optional<ChildType> {};
  }

  static auto
  range (ContainerType& children)
  {
    auto& vector_children = std::get<VType> (children);
    return Utils::Range<typename VType::iterator> (std::begin (vector_children), std::end (vector_children));
  }

  static auto
  range (ContainerType const& children)
  {
    auto const& vector_children = std::get<VType> (children);
    return Utils::Range<typename VType::const_iterator> (std::cbegin (vector_children), std::cend (vector_children));
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
      auto const name_attr = n.attribute(Name::raw.data ());

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
  template <typename ChildType>
  using MapType = MapChildBaseImpl<ChildType, Name>;

  template <typename ChildType>
  using VectorMapType = VectorMapChildBaseImpl<ChildType, Name>;
};

template <typename NameString, typename ChildType, template <typename> class ChildImpl>
class GenericChild
{
public:
  using Name = NameString;
  using Type = ChildType;
  using ImplType = ChildImpl<Type>;
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

template <typename NameString, typename ChildType>
using SingleChild = GenericChild<NameString, ChildType, SingleChildImpl>;

template <typename NameString, typename ChildType>
using VectorChild = GenericChild<NameString, ChildType, VectorChildImpl>;

template <typename NameString, typename ChildType, typename Name>
using MapChild = GenericChild<NameString, ChildType, GetKeyedChildImpl<Name>::template MapType>;

template <typename NameString, typename ChildType, typename Name>
using VectorMapChild = GenericChild<NameString, ChildType, GetKeyedChildImpl<Name>::template VectorMapType>;

} // namespace Xml

} // namespace Mm

#endif // MM_XML_GENERIC_CHILD_HH
