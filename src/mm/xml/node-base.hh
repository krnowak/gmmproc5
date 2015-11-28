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
template <typename ChildType>
using OptionalChildImplPredicate = ChildImplTagPredicate<OptionalChildTag>::template P<ChildType>;

// node base code

using HasText = std::true_type;
using NoText = std::false_type;

template <typename AttrList, typename ChildList, typename HasTextType = NoText>
class NodeBase
{
public:
  using AllAttrNames = Kr::ForEachT<AttrList, GetAttrName>;
  using AllAttrTypes = Kr::ForEachT<AttrList, GetAttrType>;

  using AllSingleChildren = Kr::SieveT<ChildList, SingleChildImplPredicate>;
  using AllVectorChildren = Kr::SieveT<ChildList, VectorChildImplPredicate>;
  using AllMapChildren = Kr::SieveT<ChildList, MapChildImplPredicate>;
  using AllVectorMapChildren = Kr::SieveT<ChildList, VectorMapChildImplPredicate>;
  using AllOptionalChildren = Kr::SieveT<ChildList, OptionalChildImplPredicate>;

  using AllSingleChildNames = Kr::ForEachT<AllSingleChildren, GetChildName>;
  using AllVectorChildNames = Kr::ForEachT<AllVectorChildren, GetChildName>;
  using AllMapChildNames = Kr::ForEachT<AllMapChildren, GetChildName>;
  using AllVectorMapChildNames = Kr::ForEachT<AllVectorMapChildren, GetChildName>;
  using AllOptionalChildNames = Kr::ForEachT<AllOptionalChildren, GetChildName>;

  using AllChildNames = Kr::ForEachT<ChildList, GetChildName>;
  using AllChildContainerTypes = Kr::ForEachT<ChildList, GetChildContainerType>;

  using AllChildAttrNames = Kr::ConcatT<AllAttrNames, AllChildNames>;
  using AllChildAttrTypes = Kr::ConcatT<AllAttrTypes, AllChildContainerTypes>;

  static_assert (Kr::IsUniqueT<AllChildAttrNames>::value, "the list of attributes and children has only unique names");

  using AllNames = AllChildAttrNames;
  using AllTypes = Kr::ConcatT<AllChildAttrTypes, std::conditional_t<HasTextType::value,
                                                                     TypeList<std::string>,
                                                                     TypeList<>>>;

  using TupleType = Kr::ToStdTupleT<AllTypes>;
  template <typename Name>
  using TupleIdxType = Kr::IndexT<Name, AllNames>;
  using ThisType = NodeBase<AttrList, ChildList, HasTextType>;
  using LastIndex = Kr::IndexType<Kr::LenT<AllTypes>::value - 1>;

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

  template <typename = void>
  class TextSetter
  {
  public:
    static void
    set (ThisType* t, Base::Node& node)
    {}
  };

  template <>
  class TextSetter<std::enable_if_t<HasTextType::value>>
  {
  public:
    static void
    set (ThisType* t, Base::Node& node)
    {
      std::get<LastIndex::value> (t->stuff) = node.text ();
      node.remove_text ();
    }
  };

public:
  template <typename Name>
  using HasStringAttribute = std::is_same<Kr::NthT<AllTypes, TupleIdxType<Name>>,
                                          std::string>;

  void
  process_node (Base::Node& node)
  {
    Kr::MultiCall<ThisType::MCAttrWrapper, AttrList>::call (this, node);
    Kr::MultiCall<ThisType::MCChildWrapper, ChildList>::call (this, node);
    ThisType::TextSetter::set (this, node);
  }

  template <typename Name>
  auto const&
  get () const
  {
    using IsAttribute = Kr::IsInListT<Name, AllAttrNames>;
    using IsSingleChild = Kr::IsInListT<Name, AllSingleChildNames>;
    using IsOptionalChild = Kr::IsInListT<Name, AllOptionalChildNames>;
    static_assert (IsAttribute::value || IsSingleChild::value || IsOptionalChild::value,
                   "the object has the attribute or the single or optional child with a given name");

    return std::get<TupleIdxType<Name>::value> (stuff);
  }

  template <typename Name>
  auto const&
  get (std::size_t idx) const
  {
    using IsVectorChild = Kr::IsInListT<Name, AllVectorChildNames>;
    using IsMapVectorChild = Kr::IsInListT<Name, AllVectorMapChildNames>;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = Kr::NthT<ChildList, ChildIndex>;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::get (std::get<ChildIndex::value> (stuff), idx);
  }

  template <typename Name>
  auto const*
  get (std::string const& key) const
  {
    using IsMapChild = Kr::IsInListT<Name, AllMapChildNames>;
    using IsMapVectorChild = Kr::IsInListT<Name, AllVectorMapChildNames>;
    static_assert (IsMapChild::value || IsMapVectorChild::value,
                   "the object has the map-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = Kr::NthT<ChildList, ChildIndex>;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::get (std::get<ChildIndex::value> (stuff), key);
  }

  template <typename = std::enable_if_t<HasTextType::value> >
  auto const&
  get_text () const
  {
    static_assert (std::is_same<NthT<AllTypes, LastIndex::value>,
                                     std::string>::value, "last element is a string");
    return std::get<LastIndex::value> (stuff);
  }

  template <typename Name>
  auto
  size () const
  {
    using IsVectorChild = Kr::IsInListT<Name, AllVectorChildNames>;
    using IsMapVectorChild = Kr::IsInListT<Name, AllVectorMapChildNames>;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = Kr::NthT<ChildList, ChildIndex>;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::size (std::get<ChildIndex::value> (stuff));
  }

  template <typename Name>
  auto const&
  range () const
  {
    using IsVectorChild = typename IsInListT<Name, AllVectorChildNames>;
    using IsMapVectorChild = Kr::IsInListT<Name, AllVectorMapChildNames>;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = Kr::NthT<ChildList, ChildIndex>;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::range (std::get<ChildIndex::value> (stuff));
  }

private:
  TupleType stuff;
};

} // namespace Xml

} // namespace Mm

#endif // MM_XML_NODE_BASE_HH
