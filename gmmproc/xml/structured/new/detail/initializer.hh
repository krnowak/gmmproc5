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

public:
  static auto
  get_tuple (Xml::Node& node)
  {
    // TODO: generate a vector with useful types, convert it to
    // std::tuple and use it as a parameter for get_tuple_impl.
    // get_tuple_impl would have a specialization for std::tuples, so
    // we will be able to access its members as a parameter pack.
    // Each parameter from the pack would be transformed and used for
    // creating a member in a storage tuple.
    return get_tuple_impl<> ();
  }
};

} // namespace Detail

} // namespace Structured

} // namespace Xml

} // namespace Gmmproc

#endif // GMMPROC_XML_STRUCTURED_DETAIL_INITIALIZER_HH
