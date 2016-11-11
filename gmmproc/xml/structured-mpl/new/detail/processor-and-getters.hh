#ifndef GMMPROC_XML_STRUCTURED_DETAIL_PROCESSOR_AND_GETTERS_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_PROCESSOR_AND_GETTERS_HH

#include <gmmproc/xml/structured/detail/storage.hh>

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename NodeSchemaP>
class ProcessorAndGetters
{
private:
  using StorageType = Storage<NodeSchemaP>;
  using AttrList = typename NodeSchemaP::AttrList;
  using ChildList = typename NodeSchemaP::ChildList;
  using PartList = Mpl::InsertRangeT<AttrList, Mpl::EndT<AttrList>, ChildList>;
  class GetUniqueGetters
  {
  public:
    template <typename MapP, typename PartP>
    class Apply
    {
    private:
      using GettersMap = Mpl::ApplyT<typename PartP::GetGetters, StorageType>;
      using GettersSet = Mpl::TransformT<GettersMap, Mpl::First<Mpl::_1>, Mpl::SetInserter>;

    public:
      using Type = Mpl::InsertRangeT<MapP, Mpl::BeginT<MapP>, GettersSet>;;
    };
  };
  using AllGetters = Mpl::FoldT<PartList, Mpl::Set<>, GetUniqueGetters>;
  using GettersSubclass = Mpl::InheritSequenceT<AllGetters>;
  // TODO: processor

public:
  using Type = GettersSubclass;
};

template <typename NodeSchemaP>
class Initializer
{
private:
  using StorageType = Storage<NodeSchemaP>;
  using AttrList = typename NodeSchemaP::AttrList;
  using ChildList = typename NodeSchemaP::ChildList;
  using PartList = Mpl::InsertRangeT<AttrList, Mpl::EndT<AttrList>, ChildList>;
  // TODO: finish it
  static
  template <typename... InitializersP>
  class Foo : private Initializers...
  {
  public:
    Foo(Xml::Node& node)
      : Initializers{node}...
    {}
  };
public:
  using Type = Foo;
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_PROCESSOR_AND_GETTERS_HH
