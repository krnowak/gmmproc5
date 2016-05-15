#ifndef GMMPROC_XML_STRUCTURED_DETAIL_INITIALIZER_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_INITIALIZER_HH

namespace Gmmproc
{

namespace Xml
{

namespace Structured
{

namespace Detail
{

template <typename NodeSchemaP>
class Initializer
{
private:
  using Helper = StorageHelper<NodeSchemaP>;
  using StorageType = Storage<NodeSchemaP>;

  class DoFoo
  {
  public:
    template <typename PartP>
    class Apply
    {
    public:
      using Type = TODO;
    };
  };
  using PartStuff = Mpl::TransformT<typename Helper::PartList,
                                    DoFoo,
                                    Mpl::VectorBackInserter>;


  template <std::size_t... IdxP>
  static auto
  get_tuple_impl (Xml::Node& node, std::index_sequence<I...>)
  {

    return std::make_tuple (Mpl::AtT<PartStuff, Utils::StdSizeTConstant<IdxP>>::generate (node)...);
  }

public:
  static auto
  get_tuple (Xml::Node& node)
  {
    return get_tuple_impl (node, std::make_index_sequence <Mpl::SizeT<PartStuff>::value>);
  }
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_INITIALIZER_HH
