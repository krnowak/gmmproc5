#ifndef MM_XML_NODE_BASE_HH
#define MM_XML_NODE_BASE_HH

#include "generic-attr.hh"
#include "generic-child.hh"

#include <mm/xml/base/xml.hh>

#include <ext/kr/kr.hh>

#include <string>
#include <tuple>
#include <type_traits>

#include <cstdlib>

namespace Mm
{

namespace Xml
{

namespace XmlDetails
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

} // namespace XmlDetails

// node base code

using HasText = std::true_type;
using NoText = std::false_type;

template <typename AttrListP, typename ChildListP, typename HasTextTypeP = NoText>
class NodeBase
{
public:
  using AttrList = AttrListP;
  using ChildList = ChildListP;
  using HasTextType = HasTextTypeP;

  // attribute names and types
  using AllAttrNames = Kr::ForEachT<AttrList, XmlDetails::GetAttrName>;
  using AllAttrTypes = Kr::ForEachT<AttrList, XmlDetails::GetAttrType>;

  // children per impl
  using AllSingleChildren = Kr::SieveT<ChildList, XmlDetails::SingleChildImplPredicate>;
  using AllVectorChildren = Kr::SieveT<ChildList, XmlDetails::VectorChildImplPredicate>;
  using AllMapChildren = Kr::SieveT<ChildList, XmlDetails::MapChildImplPredicate>;
  using AllVectorMapChildren = Kr::SieveT<ChildList, XmlDetails::VectorMapChildImplPredicate>;
  using AllOptionalChildren = Kr::SieveT<ChildList, XmlDetails::OptionalChildImplPredicate>;

  // child names per impl
  using AllSingleChildNames = Kr::ForEachT<AllSingleChildren, XmlDetails::GetChildName>;
  using AllVectorChildNames = Kr::ForEachT<AllVectorChildren, XmlDetails::GetChildName>;
  using AllMapChildNames = Kr::ForEachT<AllMapChildren, XmlDetails::GetChildName>;
  using AllVectorMapChildNames = Kr::ForEachT<AllVectorMapChildren, XmlDetails::GetChildName>;
  using AllOptionalChildNames = Kr::ForEachT<AllOptionalChildren, XmlDetails::GetChildName>;

  // child names and container types
  using AllChildNames = Kr::ForEachT<ChildList, XmlDetails::GetChildName>;
  using AllChildContainerTypes = Kr::ForEachT<ChildList, XmlDetails::GetChildContainerType>;

  // attribute and child names and types
  using AllChildAttrNames = Kr::ConcatT<AllAttrNames, AllChildNames>;
  using AllChildAttrTypes = Kr::ConcatT<AllAttrTypes, AllChildContainerTypes>;

  static_assert (Kr::IsUniqueT<AllChildAttrNames>::value, "the list of attributes and children has only unique names");

  using AllNames = AllChildAttrNames;
  using AllMixed = Kr::ConcatT<AttrList, ChildList>;
  using AllTypes = Kr::ConcatT<AllChildAttrTypes, std::conditional_t<HasTextType::value,
                                                                     Kr::TypeList<std::string>,
                                                                     Kr::TypeList<>>>;

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

  class TextSetterImpl
  {
  public:
    static void
    set (ThisType* t, Base::Node& node)
    {
      std::get<LastIndex::value> (t->stuff) = node.text ();
      node.remove_text ();
    }
  };

  class TextSetterNoop
  {
  public:
    static void
    set (ThisType*, Base::Node&)
    {}
  };

  using TextSetter = std::conditional_t<HasTextType::value,
                                        TextSetterImpl,
                                        TextSetterNoop>;

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
    using ChildType = Kr::NthT<AllMixed, ChildIndex>;
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
    using ChildType = Kr::NthT<AllMixed, ChildIndex>;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::get (std::get<ChildIndex::value> (stuff), key);
  }

  auto const&
  get_text () const
  {
    static_assert (HasTextType::value, "the object has a text node");
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
    using ChildType = Kr::NthT<AllMixed, ChildIndex>;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::size (std::get<ChildIndex::value> (stuff));
  }

  template <typename Name>
  auto
  range () const
  {
    using IsVectorChild = Kr::IsInListT<Name, AllVectorChildNames>;
    using IsMapVectorChild = Kr::IsInListT<Name, AllVectorMapChildNames>;
    static_assert (IsVectorChild::value || IsMapVectorChild::value,
                   "the object has the vector-like child with a given name");

    using ChildIndex = TupleIdxType<Name>;
    using ChildType = Kr::NthT<AllMixed, ChildIndex>;
    using ChildImplType = typename ChildType::ImplType;
    return ChildImplType::range (std::get<ChildIndex::value> (stuff));
  }

private:
  TupleType stuff;
};

template <typename ChildName, typename ChildTag>
using Root = NodeBase<Kr::TypeList<>, Kr::TypeList<SingleChild<ChildName, ChildTag>>, NoText>;

} // namespace Xml

} // namespace Mm

#endif // MM_XML_NODE_BASE_HH
