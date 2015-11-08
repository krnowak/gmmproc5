#ifndef MM_XML_NODE_BASE_HH
#define MM_XML_NODE_BASE_HH

#include <string>
#include <tuple>
#include <type_traits>

#include <cstdlib>

#include <ext/kr/kr.hh>

#include "base/xml.hh"
#include "generic-attr.hh"
#include "generic-child.hh"

namespace Mm
{

namespace Xml
{

// attr type getters

template <typename AttrType>
class GetAttrName
{
public:
  using Type = typename AttrType::Name;
};

template <typename AttrType>
class GetAttrType
{
public:
  using Type = typename AttrType::Type;
};

// child type getters

template <typename GenericChildType>
class GetChildName
{
public:
  using Type = typename GenericChildType::Name;
};

template <typename GenericChildType>
class GetChildContainerType
{
public:
  using Type = typename GenericChildType::ImplType::ContainerType;
};

template <typename ImplTag>
class ChildImplTagPredicate
{
public:
  template <typename GenericChildType>
  class P
  {
  public:
    using Type = typename std::is_same<typename GenericChildType::ImplTag,
                                       ImplTag>::type;
  };
};

template <typename ChildType>
using SingleChildImplPredicate = ChildImplTagPredicate<SingleChildTag>::template P<ChildType>;
template <typename ChildType>
using VectorChildImplPredicate = ChildImplTagPredicate<VectorChildTag>::template P<ChildType>;
template <typename ChildType>
using MapChildImplPredicate = ChildImplTagPredicate<MapChildTag>::template P<ChildType>;
template <typename ChildType>
using VectorMapChildImplPredicate = ChildImplTagPredicate<VectorMapChildTag>::template P<ChildType>;

// node base code

template <typename AttrList, typename ChildList>
class NodeBase
{
public:
  using AllAttrNames = typename Kr::ForEach<AttrList, GetAttrName>::Type;
  using AllAttrTypes = typename Kr::ForEach<AttrList, GetAttrType>::Type;

  using AllSingleChildren = typename Kr::Sieve<ChildList, SingleChildImplPredicate>::Type;
  using AllVectorChildren = typename Kr::Sieve<ChildList, VectorChildImplPredicate>::Type;
  using AllMapChildren = typename Kr::Sieve<ChildList, MapChildImplPredicate>::Type;
  using AllVectorMapChildren = typename Kr::Sieve<ChildList, VectorMapChildImplPredicate>::Type;

  using AllSingleChildNames = typename Kr::ForEach<AllSingleChildren, GetChildName>::Type;
  using AllVectorChildNames = typename Kr::ForEach<AllVectorChildren, GetChildName>::Type;
  using AllMapChildNames = typename Kr::ForEach<AllMapChildren, GetChildName>::Type;
  using AllVectorMapChildNames = typename Kr::ForEach<AllVectorMapChildren, GetChildName>::Type;

  using AllChildNames = typename Kr::ForEach<ChildList, GetChildName>::Type;
  using AllChildContainerTypes = typename Kr::ForEach<ChildList, GetChildContainerType>::Type;

  using AllNames = typename Kr::Concat<AllAttrNames, AllChildNames>::Type;
  using AllTypes = typename Kr::Concat<AllAttrTypes, AllChildContainerTypes>::Type;

  static_assert (Kr::IsUnique<AllNames>::Type::value, "the list of attributes and children has only unique names");

  using TupleType = typename Kr::ToStdTuple<AllTypes>::Type;
  template <typename Name>
  using TupleIdxType = typename Kr::Index<Name, AllNames>::Type;
  using ThisType = NodeBase<AttrList, ChildList>;

  template <typename Attr>
  class MCAttrWrapper
  {
  public:
    static void
    call (ThisType* t, Base::Node& node)
    {
      using AttrIndex = ThisType::TupleIdxType<typename Attr::Name>;
      Attr::set (std::get<AttrIndex::value> (t->stuff), node);
    }
  };

  template <typename Child>
  class MCChildWrapper
  {
  public:
    static void
    call (ThisType* t, Base::Node& node)
    {
      using ChildIndex = ThisType::TupleIdxType<typename Child::Name>;
      Child::set (std::get<ChildIndex::value> (t->stuff), node);
    }
  };

public:
  template <typename Name>
  using HasStringAttribute = std::is_same<typename Kr::Nth<AllTypes, TupleIdxType<Name>>::Type,
                                          std::string>;

  void
  process_node (Base::Node& node)
  {
    Kr::MultiCall<ThisType::MCAttrWrapper, AttrList>::call (this, node);
    Kr::MultiCall<ThisType::MCChildWrapper, ChildList>::call (this, node);
  }

  template <typename Name>
  auto
  get () const
  {
    using IsAttribute = typename Kr::IsInList<Name, AllAttrNames>::Type;
    using IsSingleChild = typename Kr::IsInList<Name, AllSingleChildNames>::Type;
    static_assert (IsAttribute::value || IsSingleChild::value,
                   "the object has the attribute or the single child with a given name");

    return std::get<TupleIdxType<Name>::value> (stuff);
  }

  template <typename Name>
  auto
  get (std::size_t idx) const
  {
    using IsVectorChild = typename Kr::IsInList<Name, AllVectorChildNames>::Type;
    using IsMapVectorChild = typename Kr::IsInList<Name, AllVectorMapChildNames>::Type;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = typename Kr::Nth<ChildList, ChildIndex>::Type;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::get (std::get<ChildIndex::value> (stuff), idx);
  }

  template <typename Name>
  auto
  get (std::string const& key) const
  {
    using IsMapChild = typename Kr::IsInList<Name, AllMapChildNames>::Type;
    using IsMapVectorChild = typename Kr::IsInList<Name, AllVectorMapChildNames>::Type;
    static_assert (IsMapChild::value || IsMapVectorChild::value,
                   "the object has the map-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = typename Kr::Nth<ChildList, ChildIndex>::Type;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::get (std::get<ChildIndex::value> (stuff), key);
  }

  template <typename Name>
  auto
  size () const
  {
    using IsVectorChild = typename Kr::IsInList<Name, AllVectorChildNames>::Type;
    using IsMapVectorChild = typename Kr::IsInList<Name, AllVectorMapChildNames>::Type;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = typename Kr::Nth<ChildList, ChildIndex>::Type;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::size (std::get<ChildIndex::value> (stuff));
  }

  template <typename Name>
  auto
  range () const
  {
    using IsVectorChild = typename Kr::IsInList<Name, AllVectorChildNames>::Type;
    using IsMapVectorChild = typename Kr::IsInList<Name, AllVectorMapChildNames>::Type;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = typename Kr::Nth<ChildList, ChildIndex>::Type;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::range (std::get<ChildIndex::value> (stuff));
  }

  template <typename Name>
  auto
  range ()
  {
    using IsVectorChild = typename Kr::IsInList<Name, AllVectorChildNames>::Type;
    using IsMapVectorChild = typename Kr::IsInList<Name, AllVectorMapChildNames>::Type;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = typename Kr::Nth<ChildList, ChildIndex>::Type;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::range (std::get<ChildIndex::value> (stuff));
  }

private:
  TupleType stuff;
};

} // namespace Xml

} // namespace Mm

#endif // MM_XML_NODE_BASE_HH
