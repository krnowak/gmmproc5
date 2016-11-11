#ifndef GMMPROC_XML_STRUCTURED_DETAIL_INITIALIZER_HH
#define GMMPROC_XML_STRUCTURED_DETAIL_INITIALIZER_HH

namespace Gmmproc::Xml::Structured::Detail
{

template <typename NodeSchemaP>
class Initializer
{
private:
  using Helper = StorageHelper<NodeSchemaP>;
  using StorageType = Storage<NodeSchemaP>;

  class GetGenerator
  {
  public:
    template <typename PartP>
    class Apply
    {
    public:
      using Type = Mpl::ApplyT<typename PartP::GetGenerator>;
    };
  };

  using PartGenerators = Mpl::TransformT<typename Helper::PartList,
                                         GetGenerator,
                                         Mpl::VectorBackInserter>;


  template <std::size_t... IdxP>
  static auto
  get_tuple_impl (Xml::Node& node, std::index_sequence<I...>)
  {
    return std::make_tuple (Mpl::AtT<PartGenerators, Utils::StdSizeTConstant<IdxP>>::generate (node)...);
  }

public:
  static auto
  get_tuple (Xml::Node& node)
  {
    return get_tuple_impl (node, std::make_index_sequence <Mpl::SizeT<PartStuff>::value>);
  }
};

} // namespace Gmmproc::Xml::Structured::Detail

#endif // GMMPROC_XML_STRUCTURED_DETAIL_INITIALIZER_HH
